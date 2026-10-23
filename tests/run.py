#!/usr/bin/env python3
"""Build all passes and check actual IR transformations and executable results."""
import pathlib
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]


def run(*args, cwd=None):
    p = subprocess.run([str(a) for a in args], cwd=cwd, text=True, capture_output=True)
    if p.returncode:
        raise RuntimeError(f'{args}\n{p.stdout}\n{p.stderr}')
    return p.stdout + p.stderr


def main():
    version = run('llvm-config', '--version').strip()
    assert version.startswith('14.'), f'Use LLVM 14, got {version}'
    folders = sorted((ROOT / 'practices/optimization').glob('[3-9]_*'))
    folders += [ROOT / 'projects/optimization/MemoryProfiler']
    for folder in folders:
        run('make', '-C', folder)
    run('make', '-C', ROOT / 'practices/optimization/Runtime')
    for folder in folders:
        run('make', '-C', folder, 'test')
    with tempfile.TemporaryDirectory() as tmp:
        tmp = pathlib.Path(tmp)

        def transform(source, folder, library, flag, runtime=(), twice=False):
            src = tmp / ('input.ll' if source.lstrip().startswith('target ') else 'input.c')
            src.write_text(source)
            bc, out, exe = (tmp / n for n in ('input.bc', 'output.bc', 'a.out'))
            run('clang', '-O0', '-Xclang', '-disable-O0-optnone', '-emit-llvm', '-c', src, '-o', bc)
            command = ['opt', '-enable-new-pm=0', '-load', folder / library, flag]
            if twice:
                command.append(flag)
            run(*command, '-verify-each', bc, '-o', out)
            run('clang++', out, *runtime, '-o', exe)
            return run(exe), run('llvm-dis', out, '-o', '-')

        opt = ROOT / 'practices/optimization'
        text, ir = transform('int inc(int); int main(){return inc(41) != 42;}',
            opt / '7_InsertIncFunction', 'InsertIncFunction.so', '-insert-inc-fun', twice=True)
        assert ir.count('define i32 @inc(') == 1
        text, _ = transform('#include <stdio.h>\nvoid printInt(int x){printf("%d\\n",x);}\n'
            'int main(){ for(int i=-3;i<=4;i++)printInt(i); return 0;}',
            opt / '8_PrintEven', 'PrintEven.so', '-print-even', twice=True)
        assert text == '-2\n0\n2\n4\n', text
        source = (opt / '9_DynamicCallCount/test.c').read_text()
        text, _ = transform(source, opt / '9_DynamicCallCount', 'DynamicCallCount.so', '-dyn-call-count',
            [opt / 'Runtime/callCount.o'], twice=True)
        assert text == 'Dynamic call count: 61\n', text
        # Include calls through a function pointer, and exit rather than main return.
        text, _ = transform('#include <stdlib.h>\nvoid f(void){}\n'
            'int main(){void (*p)(void)=f;p();exit(0);}',
            opt / '9_DynamicCallCount', 'DynamicCallCount.so', '-dyn-call-count',
            [opt / 'Runtime/callCount.o'])
        assert text == 'Dynamic call count: 2\n', text
        mem = ROOT / 'projects/optimization/MemoryProfiler'
        # Hand-written IR makes exact dynamic access counts independent of -O0 spill slots.
        source = '''target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"
declare i8* @malloc(i64)
declare void @free(i8*)
define i32 @main() {
  %p = call i8* @malloc(i64 4)
  %q = bitcast i8* %p to i32*
  store i32 42, i32* %q
  %v = load i32, i32* %q
  call void @free(i8* %p)
  %ok = icmp eq i32 %v, 42
  %rc = select i1 %ok, i32 0, i32 1
  ret i32 %rc
}
'''
        text, _ = transform(source, mem, 'MemoryProfiler.so', '-mem-prof', [mem / 'runtime.cpp'], twice=True)
        assert text == 'Memory profile: malloc=1 bytes=4 free=1 load=1 bytes=4 store=1 bytes=4\n', text
        run('clang', '-fsanitize=address,undefined', mem / 'test.c', '-o', tmp / 'memory-sample')
        run(tmp / 'memory-sample')
        # An existing incompatible inc declaration must fail with a clear error.
        (tmp / 'bad.ll').write_text('declare void @inc()\n')
        p = subprocess.run(['opt', '-enable-new-pm=0', '-load', str(opt / '7_InsertIncFunction/InsertIncFunction.so'),
            '-insert-inc-fun', str(tmp / 'bad.ll'), '-disable-output'], capture_output=True, text=True)
        assert p.returncode != 0 and 'inc must have signature' in p.stderr
    print(f'PASS: all 8 passes built; IR verification, even/odd/negative values, increment, '
          f'dynamic calls, indirect calls, exit(), memory byte counts, idempotence, and ASan/UBSan (LLVM {version})')


if __name__ == '__main__':
    main()
