from pathlib import Path
import subprocess
import sys


def source_root() -> Path:
    return Path(__file__).resolve().parents[2]


def ninja_binary() -> Path:
    candidate = Path.cwd() / 'ninja'
    if not candidate.is_file():
        raise AssertionError(f'could not find built ninja at {candidate}')
    return candidate


def test_posix_runner_has_direct_exec_fast_path() -> None:
    source = (source_root() / 'src' / 'subprocess-posix.cc').read_text(encoding='utf-8')

    assert 'ParseDirectCommand' in source
    assert 'direct_argv' in source
    assert 'posix_spawn(&pid_, direct_argv[0]' in source
    # Shell syntax must retain Ninja's established fallback behavior.
    assert '"/bin/sh", "-c", command.c_str()' in source


def test_shell_dependent_command_keeps_working(tmp_path: Path) -> None:
    output = tmp_path / 'shell-output.txt'
    manifest = tmp_path / 'build.ninja'
    manifest.write_text(
        'rule shell\n'
        f"  command = printf '%s\\n' direct shell > {output}\n"
        'build result: shell\n',
        encoding='utf-8',
    )

    completed = subprocess.run(
        [str(ninja_binary()), '-f', str(manifest), 'result'],
        cwd=tmp_path,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout
    assert output.read_text(encoding='utf-8') == 'direct\nshell\n'
