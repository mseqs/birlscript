# BIRLSCRIPT v2.x ALPHA

## Especificação

Nesse arquivo contém as especificações padrões sobre como a linguagem deve ser estruturada,
o que são seus tipos, semânticas e outras coisas que fazem parte do ecossistema.

Mais sobre a linguagem pode ser encontrado no README, enquanto aqui ficam mais informações
tecnicas sobre o funcionamento interno do interpretador/compilador da linguagem.

Contato: [lazpeng@gmail.com](mailto:lazpeng@gmail.com)

## Semânticas

### Comentários
* Código
```
# Comentários duram apenas uma linha e se iniciam com '#'
# Tudo depois disso é completamente ignorado
```
* Funcionamento

O parser simplesmente deve considerar a linha como "útil" ou
"utilizável" até a aparição do caractere '#'. Todo o conteúdo
da linha deve ser ignorado depois disso.

### Inicio do programa
* Código
```
# Código global. É executado antes, mas somente uma vez

HORA DO SHOW # O mesmo que declarar uma função de nome SHOW
# Codigo
BIRL
```
* Funcionamento

Todo programa começa com a execução de duas coisas principais:
O contexto ou função "global" (que é onde ficam os comandos globais)
e a "função" principal, que começa com *"HORA DO SHOW"* e termina, **como
qualquer bloco** com *"BIRL"*. Como especificado no código,
uma função definida com o identificador "SHOW" também é válida
como ponto de partida, então recursão na função principal é
alcançada passando a execução pra essa função "SHOW" com a
devida sintaxe.

### Variáveis
* Código
```
VEM nome COMO tipo # Valor: indefinido: Tipo explícito
VEM nome QUE É <valor> # Valor: explícito. Tipo implícito
VEM nome COMO tipo QUE É <valor> # Valor e tipo explícitos
VEM nome QUE É <valor> COMO tipo # Tanto faz a ordem
```
* Funcionamento

Variáveis são como objetos que guardam um valor e ficam
disponíveis na "stack" da função/ambiente atual (e somente lá),
que possuem valores que podem ser modificados que são ligados
a um identificador que, como dito, só é válido (pra aquele
valor em específico) no ambiente atual.

Quando um tipo não é
passado explicitamente para a declaração da variável, ele é
"adivinhado" pelo interpretador e se torna estático desde então,
ou seja, nenhum valor de outro tipo senão o original é permitido
ser guardado por essa variável. Você pode também ser explícito
com o tipo da variável, que funciona como um tipo de conversão
***entre tipos compatíveis***.

### Globais
* Código
```
<indefinido>
```
* Funcionamento

Há dois tipos de globais (isto é, objetos que guardam um valor,
disponiveis no contexto global): Os globais constantes, declarados
com sua própria sintaxe e valor invariável, e as variáveis
definidas no código global. Essas funcionam como variáveis comuns
e, portanto, não estão disponíveis na stack de outras funções,
sendo assim a única forma de serem modificadas (senão no global
em si) é colocando nelas o valor de algum retorno de função
ou ~~passando ela como referência para alguma função a modificar~~(?).

### Estruturas
* Código
```
JAULA identificador
variavel_1 <tipo> # Nome e tipo dos campos.
# Não é necessária virgula ou qualquer coisa do tipo
# na separação dos campos, apenas uma nova linha
...
SAINDO DA JAULA # Finaliza a declaração da estrutura

# A função de criação dessa função seria
# cria_identificador(v_1: <tipo>)
```

* Funcionamento

As estruturas são um tipo definido pelo usuário e então tem
um comportamento diferente dos tipos padrão, que não são
inicializados automaticamente (têm um valor dependente do
objeto anterior na stack, pode ser qualquer coisa, se não
devidamente inicializado). Esse comportamento pode ser 
configurado na estrutura, caso o usuário não queira que
o objeto seja inicializado com valores padrão no memento
da criação de um novo objeto desse tipo.

Por padrão, é criada uma função que tem o nome de *"cria_"* +
o nome da estrutura pra se fazer a devida criação do objeto,
como mostrado no exemplo. Quando o valor do objeto for implícito
(como na criação de uma variável onde somente o tipo é especificado),
no caso de ele ser inicializado automaticamente (que é o padrão),
esse "construtor" seria chamado com os valores padrão pra cada
campo.

### Funções
* Código
```
<indefinido>
```

* Funcionamento

Funções contém um identificador (pra que possam ser "chamados"
de outros lugares do programa), uma lista de parâmetros opcional,
um tipo de retorno também opcional e um corpo, que é onde
ficam os comandos e toda a funcionalidade do programa. Quando
não especificada uma lista de argumentos, ela é vazia. Isso é,
não é necessário nenhum argumento pra se chamar a função. Quando
não é especificado um tipo de retorno, significa não só que a
função não retorna valor algum, como também que ela é proibida
de retornar qualquer valor. Tendo isso em mente, é inválido
dar a uma variável que possui um tipo válido (diferente de
nulo/vazio) a chamada/retorno de uma função que retorna... nada.