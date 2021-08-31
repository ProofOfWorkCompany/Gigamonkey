# Gigamonkey

A library with basic Bitcoin functions. 

## Instructions to build and upload a conan package from the recipe

    $ conan create . proofofwork/stable
    $ conan remote add proofofwork https://pow.jfrog.io/artifactory/api/conan/proofofwork
    $ conan user -p PASSWORD -r proofofwork USERNAME
    $ conan upload gigamonkey/0.1@proofofwork/stable --all -r proofofwork --confirm

## Run conan build in specific local directory

    $ mkdir build
    $ conan install . -if build
    $ conan build . -bf build

## Dependencies

* The Bitcoin SV reference implementation. 
* My library Data which contains tools for high level programming, which in turn depends on.
    * GMP and GMP++
    * ntl
    * ctre
* libsecp256k1
* crypto++

## Overview

* wallet.hpp - a basic wallet. 
* merkle.hpp - Merkle trees and paths. 
* schema/ - key schema. Right now there is HD and random. 
* boost/ - Boost outputs

## Future Plans

* encryption / decryption
* B, Bcat
* paymails

## Name

Gigamonkey is my name for the Bitcoin egregore. 
