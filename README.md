NISTCrypto

A collection of FIPS cryptographic algorithm implementations in C++. Tested on Ubuntu 22.04. Supported algorithms:

    SHA2-224/256/384/512     (https://csrc.nist.gov/pubs/fips/180-4/upd1/final)
    SHA3-224/256/384/512     (https://csrc.nist.gov/pubs/fips/202/final)
    AES-ECB/CBC/CFB/OFB      (https://csrc.nist.gov/pubs/fips/197/final)

File structure:

    NISTCrypto/
    |
    |--src/     # Algorithm and helper source.
    |--inc/     # Headers
    |--test/    # Test source and folders of NIST test vectors.

Algorithm tests below use NIST test vectors available at https://csrc.nist.gov/projects/cryptographic-algorithm-validation-program/.

Build:

    docker build -t nistcrypto .
    docker run --rm -it -v "$PWD":/app -w /app nistcrypto /bin/bash
    make release

Usage:  

    ./NISTCrypto [-r <x,y,z...>] [-o <path>]
    
    -r    Run a comma-separated list of numbered test groups, e.g., '-r 1,3,4'. See test groups below.
    -o    Output test log file.

Available Test Groups:

    1 - BigInt: Arbitrary size integer unit tests. BigInts used for cryptographic algorithms.
    2 - Hash: SHA2-224/256/384/512 and SHA3-224/256/384/512 tests.
    3 - Block Cipher: AES ECB/CBC/CFB/OFB tests.
