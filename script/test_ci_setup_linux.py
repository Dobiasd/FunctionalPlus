"""Run with python3 -m unittest discover -s script -p test_ci_setup_linux.py.

Uses real APT against a task-local file repository in download-only mode.
No host package state, global APT configuration, network, or installation is used.
"""
import hashlib
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


SCRIPT = Path(__file__).with_name("ci_setup_linux.sh")


@unittest.skipUnless(all(shutil.which(tool) for tool in ["apt-get", "dpkg-deb", "bash"]), "requires Linux APT tools")
class SetupTests(unittest.TestCase):
    def run_setup(self, installed):
        with tempfile.TemporaryDirectory(prefix="fplus-apt-test-") as temporary:
            root = Path(temporary)
            for name in ["etc/apt.conf.d", "etc/sources.list.d", "etc/trusted.gpg.d", "etc/preferences.d", "state/lists/partial", "cache/archives/partial", "log", "repo/pool", "bin"]:
                (root / name).mkdir(parents=True, exist_ok=True)
            artifact = root / "repo/pool/git_2.0_amd64.deb"
            size, digest = 1, hashlib.sha256(b"x").hexdigest()
            if not installed:
                control = root / "package/DEBIAN/control"
                control.parent.mkdir(parents=True)
                control.write_text("Package: git\nVersion: 2.0\nArchitecture: amd64\nMaintainer: Synthetic test <test@example.invalid>\nDescription: synthetic CI setup fixture\n", encoding="utf-8")
                subprocess.run([shutil.which("dpkg-deb"), "--build", str(control.parent.parent), str(artifact)], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                data = artifact.read_bytes()
                size, digest = len(data), hashlib.sha256(data).hexdigest()
            # Installed case deliberately advertises an unavailable newer artifact.
            (root / "repo/Packages").write_text(f"Package: git\nVersion: 2.0\nArchitecture: amd64\nFilename: pool/git_2.0_amd64.deb\nSize: {size}\nSHA256: {digest}\nDescription: synthetic CI setup fixture\n\n", encoding="utf-8")
            (root / "etc/sources.list").write_text(f"deb [trusted=yes] { (root / 'repo').as_uri() } ./\n", encoding="utf-8")
            status = root / "state/status"
            status.write_text("Package: git\nStatus: install ok installed\nArchitecture: amd64\nVersion: 1.0\nDescription: synthetic installed fixture\n\n" if installed else "", encoding="utf-8")
            config = root / "apt.conf"
            config.write_text(f'''Dir "{root}";
Dir::Etc "{root}/etc";
Dir::Etc::parts "{root}/etc/apt.conf.d";
Dir::Etc::main "{root}/etc/apt.conf";
Dir::Etc::sourcelist "{root}/etc/sources.list";
Dir::Etc::sourceparts "{root}/etc/sources.list.d";
Dir::Etc::trusted "{root}/etc/trusted.gpg";
Dir::Etc::trustedparts "{root}/etc/trusted.gpg.d";
Dir::State "{root}/state";
Dir::State::status "{root}/state/status";
Dir::State::lists "{root}/state/lists";
Dir::Cache "{root}/cache";
Dir::Cache::archives "{root}/cache/archives";
Dir::Log "{root}/log";
APT::Architecture "amd64";
Acquire::Languages "none";
APT::Get::Download-Only "true";
Debug::NoLocking "true";
''', encoding="utf-8")
            wrapper = root / "bin/apt-get"
            wrapper.write_text(f'#!/bin/sh\nexec "{shutil.which("apt-get")}" "$@"\n', encoding="utf-8")
            wrapper.chmod(0o700)
            env = {key: value for key, value in os.environ.items() if not key.startswith(("APT_", "DEBIAN_"))}
            env.update(APT_CONFIG=str(config), PATH=str(root / "bin") + os.pathsep + os.environ["PATH"], LC_ALL="C", HOME=str(root))
            before = status.read_bytes()
            result = subprocess.run([shutil.which("bash"), str(SCRIPT)], cwd=root, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, timeout=30)
            self.assertEqual(status.read_bytes(), before)
            return result

    def test_existing_git_does_not_need_newer_artifact(self):
        result = self.run_setup(installed=True)
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertIn("Skipping git", result.stdout)

    def test_missing_git_still_selects_installation(self):
        result = self.run_setup(installed=False)
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertIn("The following NEW packages will be installed", result.stdout)
        self.assertIn("Download complete and in download only mode", result.stdout)


if __name__ == "__main__":
    unittest.main()
