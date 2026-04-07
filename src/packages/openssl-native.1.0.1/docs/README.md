## About

This package is a binary distribution of [OpenSSL](https://www.openssl.org/) LTS version 1.1.1q.

It ships OpenSSL components (targeting both x86 and x64 architectures) :
- Dynamic libraries 
- Static libraries 
- PDB files
- OpenSSL executables

These libraries have no external runtime dependencies (except OS libraries) as they statically link against the MS C runtime (vc143).

It has been built from the [OpenSSL 1.1.1q sources](https://github.com/openssl/openssl/releases/tag/OpenSSL_1_1_1q) using the following environment:
- Windows 10 20H2 (build 19042.1766)
- Viusal Stutdio 2022 17.2.6
- NASM 2.15.05
- Strawberry Perl 5.32.1.1
- Perl module Text::Template 1.61

## Confuring OpenSSL libraries linkage mode 

By default, after package intallation, default behavior is to link against OpenSSL dynamic libraries.

To change this behavior, you can go to your project Properties under 'Configuration Properties' > 'Referenced Packages' > 'OpenSSL' > 'Linkage':
- 'Dynamic Library (DLL)' : default behavior. It configures your project to link against OpenSSL dynamic libraries using import libraries.
- 'Static' : It configures your project to link against OpenSSL static libraries that will be embedded in your libary/executable.

## Notes

OpenSSL.exe is also included in the packages for optional utilitary purpose. 
It is located in the nuget package folder under 'build\native\bin\[x86|x64]'.

## License

OpenSSL version 1.1.1q is licensed under the open source [dual OpenSSL and SSLeay license](https://https://github.com/openssl/openssl/blob/OpenSSL_1_1_1q/LICENSE).
A local copy of the licence (license-openssl-ssleay.txt) can be found in the nuget package folder '/docs'



