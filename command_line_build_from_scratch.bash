#!/bin/bash
workspace=$(mktemp -d)
project=$(dirname $(readlink -f $0))
eclipse -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data $workspace -import $project -build all
rm -rf $workspace

