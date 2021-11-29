#!/bin/bash -e

FILE=$1
GITHUB_TOKEN=

curl -X POST \
    -H "Content-Length: $(stat --format=%s $FILE)" \
    -H "Content-Type: $(file -b --mime-type $FILE)" \
    -T "$FILE" \
    -H "Authorization: token $GITHUB_TOKEN" \
    -H "Accept: application/vnd.github.v3+json" \
    "https://uploads.github.com/repos/bareboat-necessities/lysmarine_gen/releases/54202060/assets?name=$(basename $FILE)" | cat

curl -X GET \
    -H "Authorization: token $GITHUB_TOKEN" \
    -H "Accept: application/vnd.github.v3+json" \
    "https://api.github.com/repos/bareboat-necessities/lysmarine_gen/releases/54202060/assets"

