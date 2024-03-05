" Vim syntax filestdin
" Language: WIS

" Usage Instructions
" Put this file in .vim/syntax/wis.vim
" and add in your .vimrc file the next line:
" autocmd BufRead, BufNewFile *.wis set filetype=wis

if exists("b:current_syntax")
  finish
endif

" Keywords
syntax keyword wisKeywords if else end while do bind use

" Comments
syntax region wisCommentLine start="//" end="$"

" Strings
syntax region wisString start=/\v"/ skip=/\v\\./ end=/\v"/
syntax region wisString start=/\v'/ skip=/\v\\./ end=/\v'/

" Set highlights
highlight default link wisKeywords Identifier
highlight default link wisCommentLine Comment
highlight default link wisString String

let b:current_syntax = "wis"