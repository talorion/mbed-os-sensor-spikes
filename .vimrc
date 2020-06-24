set nocompatible              " be iMproved, required
filetype off                  " required

" set the runtime path to include Vundle and initialize
set rtp+=./.vim/bundle/Vundle.vim
call vundle#begin('./.vim/bundle')

" let Vundle manage Vundle, required
Plugin 'VundleVim/Vundle.vim'

" This plugin takes the advantage of new apis in Vim 8 (and NeoVim) to enable you to run shell commands in background and read output in the quickfix window in realtime
Plugin 'skywind3000/asyncrun.vim'

" YouCompleteMe is a fast, as-you-type, fuzzy search code completion engine for Vim
Plugin 'valloric/youcompleteme'

" Full path fuzzy file buffer, mru, tag, ... finder for vim
 Plugin 'ctrlpvim/ctrlp.vim'

"Easily toggle between *.c* and *.h* buffers.
Plugin 'ericcurtin/CurtineIncSw.vim'

" Comment functions so powerful—no comment necessary.
Plugin 'scrooloose/nerdcommenter'

" Easily interact with tmux from vim.
Plugin 'benmills/vimux'

" vim-scrum-markdown depends on vim-markdown and vim-markdown-wiki
Plugin 'mmai/vim-markdown-wiki'

" vim-scrum-markdown is a Vim plugin to manage a scrum dashboard inside vim
Plugin 'mmai/vim-scrum-markdown'

"  Vim plugin to make working with CMake a little nicer.
Plugin 'vhdirk/vim-cmake'

" All of your Plugins must be added before the following line
call vundle#end()            " required
filetype plugin indent on    " required

" Enable syntax highlighting
syntax on
filetype plugin indent on

" Colorscheme see https://github.com/hukl/Smyck-Color-Scheme
color smyck

" Add line numbers
set number
set ruler
set cursorline

" Disable Backup and Swap files
set noswapfile
set nobackup
set nowritebackup

" Set encoding
set encoding=utf-8

" Whitespace stuff
set nowrap
set tabstop=4
set shiftwidth=4
set softtabstop=4
set expandtab

" highlight all occurrence of a search
set hlsearch

" Strip trailing whitespaces on each save
fun! <SID>StripTrailingWhitespaces()
    let l = line(".")
    let c = col(".")
    %s/\s\+$//e
    call cursor(l, c)
endfun
autocmd BufWritePre * :call <SID>StripTrailingWhitespaces()

" Remove newbie crutches in Command Mode
cnoremap <Down> <Nop>
cnoremap <Left> <Nop>
cnoremap <Right> <Nop>
cnoremap <Up> <Nop>

" Remove newbie crutches in Insert Mode
inoremap <Down> <Nop>
inoremap <Left> <Nop>
inoremap <Right> <Nop>
inoremap <Up> <Nop>

" Remove newbie crutches in Normal Mode
nnoremap <Down> <Nop>
nnoremap <Left> <Nop>
nnoremap <Right> <Nop>
nnoremap <Up> <Nop>

" Remove newbie crutches in Visual Mode
vnoremap <Down> <Nop>
vnoremap <Left> <Nop>
vnoremap <Right> <Nop>
vnoremap <Up> <Nop>

" highlight column number 110 with color.
set colorcolumn=110
highlight ColorColumn ctermbg=darkgray

" Disable Mode Display because Status line is on
set noshowmode

" Show trailing spaces and highlight hard tabs
set list listchars=tab:»·,trail:·

" allow backspacing over everything in insert mode
set backspace=indent,eol,start

" Setup netrw
let g:netrw_liststyle = 3
let g:netrw_banner = 0
let g:netrw_browse_split = 0
let g:netrw_altv = 1
let g:netrw_winsize = 25
"augroup ProjectDrawer
    "autocmd!
    "autocmd VimEnter * :Vexplore
"augroup END
let g:NetrwIsOpen=0
function! NetrwToggle()
    if g:NetrwIsOpen
        let i = bufnr("$")
        while (i >= 1)
            if (getbufvar(i, "&filetype") == "netrw")
                silent exe "bwipeout " . i
            endif
            let i-=1
        endwhile
        let g:NetrwIsOpen=0
    else
        let g:NetrwIsOpen=1
        silent Lexplore
    endif
endfunction

" Add your own mapping. For example:
noremap <silent> <F2> :call NetrwToggle()<CR>

" You could try setting the g:termdebugger variable to the gdb you want to run
let g:termdebugger = '/opt/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-gdb'
" let g:termdebug_wide = 50
let g:termdebug_use_prompt = 1
packadd termdebug

let g:cmake_export_compile_commands = 1
let g:cmake_ycm_symlinks = 1
let g:cmake_build_type = 'Debug'
let g:cmake_project_generator ='Ninja'
" let g:cmake_build_dir = '.'
" set makeprg=make\ -C\ ../build\ -j12
set makeprg=ninja\ -C\ ./build

" map <F2> for NERDTreeToggle
" nnoremap <silent> <F2> : NERDTreeToggle<CR>

" map <F4> for toggle heder source
nnoremap <silent> <F4> : call CurtineIncSw()<CR>

function! StartOrContinueDebug()
    let bnr = bufwinnr(g:termdebugger)
    if bnr > 0
        :Continue
    else
        :Termdebug
        :exe bnr . "wincmd R"
        :exe bnr . "wincmd R"
        ":exe bnr . "wincmd N"
        :bd! gdb\ program
        :res -20
    endif

endfunction

" map <F5> for Start Debugging
" nnoremap <silent> <F5> :Continue <CR>
nnoremap <silent> <F5> :call StartOrContinueDebug() <CR>

" map <F9> for Toggle breakpoint
" let g:ConqueGdb_ToggleBreak = g:ConqueGdb_Leader . 'b'
" let g:ConqueGdb_ToggleBreak = '<F9>'
nnoremap <silent> <F9> :Break <CR>

" map <F10> for Step over
" let g:ConqueGdb_Next = g:ConqueGdb_Leader . 'n'
"let g:ConqueGdb_Next = '<F10>'
" nnoremap <silent> <F10> :ConqueGdbCommand source .gdb/step_over.gdb<CR>
nnoremap <silent> <F10> :Over <CR>

" map <F11> for Step into
" let g:ConqueGdb_Step = g:ConqueGdb_Leader . 's'
"let g:ConqueGdb_Step = '<F11>'
" nnoremap <silent> <F11> :ConqueGdbCommand source .gdb/step_into.gdb<CR>
nnoremap <silent> <F11> :Step <CR>

" map leader <F11> for Step out
" let g:ConqueGdb_Finish = g:ConqueGdb_Leader . 'f'
"let g:ConqueGdb_Finish = '<S-F11>'
" nnoremap <silent> <S-F11> :ConqueGdbCommand source .gdb/step_out.gdb<CR>
nnoremap <silent> <S-F11> :Finish <CR>

" map <F5> for continue
" let g:ConqueGdb_Continue = g:ConqueGdb_Leader . 'c'
" nnoremap <silent> <F5> :Continue <CR>

" map <F7> for make
" ! mark prevents Vim from jumping to location of first error found
nnoremap <F7> :make!<cr>
" nnoremap <silent> <F7> :VimuxRunCommand("ninja -C build -j12 ")<CR>

" map <F9> for Apply YCM FixIt
" map <F9> :YcmCompleter FixIt<CR>

" map <F12> for GoToDefinition
nnoremap <silent> <F12> :YcmCompleter GoTo<CR>

" nnoremap <C-s> :w
noremap <Leader>s :wa<CR>

" nnoremap <C-S> :wa
" noremap <Leader>s :wa<CR>
nnoremap <silent> <Leader>t :!ctest --output-on-failure<CR>

nnoremap <Tab> :tabn<cr>

" run ctags when c/c++ file is saved
function! DelTagOfFile(file)
    let fullpath = a:file
    let cwd = getcwd()
    let tagfilename = cwd . "/tags"
    let f = substitute(fullpath, cwd . "/", "", "")
    let f = escape(f, './')
    let cmd = 'sed -i "/' . f . '/d" "' . tagfilename . '"'
    let resp = system(cmd)
endfunction

function! UpdateTags()
    let f = expand("%:p")
    let cwd = getcwd()
    let tagfilename = cwd . "/tags"
    let cmd = 'ctags -a -f ' . tagfilename . ' --c++-kinds=+p --fields=+iaS --extra=+q ' . '"' . f . '"'
    call DelTagOfFile(f)
    let resp = system(cmd)
endfunction

autocmd BufWritePost *.cpp,*.h,*.c call UpdateTags()

