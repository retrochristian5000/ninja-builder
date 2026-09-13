import os
from pathlib import Path
import subprocess
import sys


DIRECT_SENTINEL = 'whp-ninja-direct-exec'


def ninja_binary() -> Path:
    candidate = Path.cwd() / 'ninja'
    if not candidate.is_file():
        raise AssertionError(f'could not find built ninja at {candidate}')
    return candidate


def test_simple_absolute_command_bypasses_posix_shell(tmp_path: Path) -> None:
    helper = tmp_path / 'record_env.py'
    output = tmp_path / 'direct-env.txt'
    helper.write_text(
        "import os, pathlib, sys\n"
        "pathlib.Path(sys.argv[1]).write_text(os.environ.get('_', ''), encoding='utf-8')\n",
        encoding='utf-8',
    )

    manifest = tmp_path / 'build.ninja'
    manifest.write_text(
        'rule record\n'
        f'  command = {sys.executable} {helper} {output}\n'
        'build result: record\n',
        encoding='utf-8',
    )

    env = os.environ.copy()
    env['_'] = DIRECT_SENTINEL
    completed = subprocess.run(
        [str(ninja_binary()), '-f', str(manifest), 'result'],
        cwd=tmp_path,
        env=env,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout
    assert output.read_text(encoding='utf-8') == DIRECT_SENTINEL
