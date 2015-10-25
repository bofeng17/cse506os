#!/bin/bash
clear
echo 'Adding Files To Local Repo...'
git add . -f
echo 'Git Add done!'
echo ''

read -p "Commit Message: " msg

echo 'Committing To GitHub...'
git commit -m "$msg"
echo 'Committing Done!'
echo ''

echo 'Pushing To GitHub ...'
git push origin master
echo 'Updated On GitHub!'
echo ''

echo 'All finished!'
echo ''