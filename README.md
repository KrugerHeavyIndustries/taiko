# Taiko OpenID Provider

Taiko is an OpenID Provider daemon written in C++ and built upon the (mongoose)[https://code.google.com/p/mongoose/] 
embedded HTTP server and (libopkele)[http://kin.klever.net/libopkele/].

On Unix and Unix like systems it authenticates users against the system password file.

## Features

- MacOSX, BSD and Linux support
- Performant low memory usage OpenID provider
- Individual users can be configured/activated for authentication/verification against Taiko
- Fullname and email address attributes presented to OpenID consumer can be configured per user. 

## Roadmap

- Ersatz password support for use cases where one does not want to use system passwords
- Authentication against LDAP/ActiveDirectory 
- TOTP (Time-based One-Time Password) support as per (rfc6238)(http://tools.ietf.org/html/rfc6238)

## Quickstart (OSX) 

After cloning the repo install premake (via homebrew - recommended)  

(in the taiko directory)

brew install premake

premake4 xcode3 

open build/taikod.xcodeproj 

Build and run

### Setting up PAM configuration for taiko 

Create a file in /etc/pam.d/ called taiko with the following in it

```
auth       required       pam_opendirectory.so
account    required       pam_opendirectory.so
```

