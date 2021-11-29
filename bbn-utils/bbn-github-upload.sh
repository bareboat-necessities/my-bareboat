#!/bin/bash -e

FILE=$1

curl \
    -H "Authorization: token $GITHUB_TOKEN" \
    -H "Content-Type: $(file -b --mime-type $FILE)" \
    --data-binary @$FILE \
    "https://uploads.github.com/repos/bareboat-necessities/lysmarine_gen/releases/50516401/assets?name=$(basename $FILE)"
