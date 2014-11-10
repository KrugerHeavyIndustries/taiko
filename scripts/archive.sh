#!/bin/sh
TAG=0.1.2
git archive --format tar.gz --output taiko_${TAG}.orig.tar.gz --prefix taiko-${TAG}/ ${TAG}
