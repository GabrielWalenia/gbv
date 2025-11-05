# gbv
Gabriel Antonio Walenia
GRR20243090

Problemas com o código: 
A função remove foi dificil de fazer e ela só funciona satisfatoriamente com arquivos pequenos

Há alguns memory leaks no código, porém eu não consegui fecha-los pois precisaria de um fclose e quando eu usava a funcao f close e 
abria de novo o arquivo, ele resultava em um segmentation fault

Alterações
Criei uma função gbv_kill no main apenas para usar o comando free no lib->docs