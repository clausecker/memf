#!/bin/sh

set -e

(cd src ; make ; )
(cd test ; make ; )
