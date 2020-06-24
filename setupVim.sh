#!/bin/sh

if [ ! -d .vim/bundle/Vundle.vim ]; then
    git clone https://github.com/VundleVim/Vundle.vim.git .vim/bundle/Vundle.vim
fi

vim +PluginInstall +qall

if [ ! -f .vim/bundle/YouCompleteMe/third_party/ycmd/ycm_core.so ]; then
	cd .vim/bundle/YouCompleteMe
	./install.py --clang-completer
	cd ../../../
fi

mkdir build

