#!/bin/bash
if [ ! -d "build" ]; then
  echo "Rien à nettoyer."
  exit 0
fi
cd build
make clean
cd ..
echo "Nettoyage du répertoire de construction..."
rm -rf build
echo "Nettoyage terminé!"
clear