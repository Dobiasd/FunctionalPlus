from conan.packager import ConanMultiPackager
import os

if __name__ == "__main__":
    if os.getenv("CXX") == "g++-7":
        version = os.getenv("TRAVIS_TAG")
        if not version:
            version = "dev"
        reference = "functionalplus/%s" % version
        username = "dobiasd"
        channel = "stable"
        upload_remote = "https://api.bintray.com/conan/dobiasd/public-conan" if version is not "dev" else None

        print("Conan package metadata:", reference, username, channel, upload_remote)

        builder = ConanMultiPackager(reference=reference,
                                    username=username,
                                    channel=channel,
                                    upload=upload_remote)
        builder.add(settings={
            'os': 'Linux',
            'compiler.version': '7',
            'compiler.libcxx': 'libstdc++11',
            'arch': 'x86_64',
            'build_type': 'Release',
            'compiler': 'gcc'
        })
        builder.run()
