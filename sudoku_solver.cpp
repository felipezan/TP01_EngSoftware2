#include <iostream>
#include <stdio.h>
#include <math.h>
#include <cstring>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <chrono>
#include <sstream>
#include <algorithm>
#include <deque>

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::string;
using std::pair;
using std::make_pair;
using std::queue;
using std::list;
using std::stringstream;
using std::reverse;
using std::priority_queue;
using std::deque;

class Node {
    private:
        Node* parent_node;
        int nivel;
        list<Node*> child_nodes;
        int matrix_i_position;
        int matrix_j_position;
        int weight;

    public:

        // construtor
        Node(int nivel_no, int i_pos, int j_pos, int weight_value) : nivel(nivel_no), matrix_i_position(i_pos), matrix_j_position(j_pos), weight(weight_value) {}

        // construtor
        Node(Node* parent, int nivel_no, int i_pos, int j_pos, int weight_value) : parent_node(parent), nivel(nivel_no), matrix_i_position(i_pos), matrix_j_position(j_pos), weight(weight_value) {}

        // destrutor
        ~Node() {
            for (auto child : child_nodes) {
                delete child;
            }
        }

        // adicionando no filho
        void addChild(Node* child) {
            child_nodes.push_back(child);
        }

        // retorna parente
        Node* getParent() const {
            return parent_node;
        }

        // retorna lista de filhos
        const list<Node*>& getChildren() const {
            return child_nodes;
        }

        // retorna posicao i na matriz
        int getIPosition() const {
            return matrix_i_position;
        }

        // retorna posicao j na matriz
        int getJPosition() const {
            return matrix_j_position;
        }

        // retorna do no na arvore
        int getNivel() const {
            return nivel;
        }

        // retorna peso
        int getWeight() const {
            return weight;
        }

        // segunda versao print tree
        void printTree2(int level = 0) const {

            stringstream ss;

            // no atual com info de nivel
            ss << "Level " << level << ": (" << weight << ")[" << matrix_i_position << "][" << matrix_j_position << "] -----> Children: ";

            // filhos do no atual
            bool first = true;
            for (auto child : child_nodes) {
                if (!first) ss << " - "; 
                ss << "(" << child->weight << ")[" << child->matrix_i_position << "][" << child->matrix_j_position << "]";
                first = false;
            }

            // printando string construida
            cout << ss.str() << endl; 
            
            // recursivamente printando nos filhos da mesma maneira
            for (auto child : child_nodes) {
                child->printTree(level + 1);
            }

        }

        // printa arvore
        void printTree(int level = 0) const {
            
            // printa no corrent
            cout << "(" << weight << ")[" << matrix_i_position << "," << matrix_j_position << "]";
            if (level == 0) cout << endl;
            
            // printa nos filhos
            for (auto child : child_nodes) {
                cout << " - ";
                child->printTree(level + 1);
            }
                cout << endl;

            // se eh o ultimo filho, printa newline
            if (level == 0)
                cout << endl;
        
        }

};


// definindo tipo matrix como vector de vectors para uma sintaxe mais clara
typedef vector<vector<int>> Matrix;

// definindo constantes
static const bool TEST_PRINT = true;
static const int QUANT_LINHAS = 9;
static const int QUANT_COLUNAS = 9;
static const int TAMANHO_SUBDVISOES_LINHAS = 3;
static const int TAMANHO_SUBDVISOES_COLUNAS = 3;
/*
static const int QUANT_LINHAS = 4;
static const int QUANT_COLUNAS = 4;
static const int TAMANHO_SUBDVISOES_LINHAS = 2;
static const int TAMANHO_SUBDVISOES_COLUNAS = 2;
 */

// definindo metodos
void printMatrix(Matrix &matriz, string mensagem);
void testPrint(string mensagem);
int breadth_first_search(Matrix &sudoku_board, vector<pair<int, int>> &vec_posicoes_vazias, int quant_posicoes_vazias, Matrix &numeros_possiveis); 
int iterative_deepening_seach(Matrix &sudoku_board, vector<pair<int, int>> &vec_posicoes_vazias, int quant_posicoes_vazias, Matrix &numeros_possiveis); 
void preencherMatrixResultado(Matrix &sudoku_board, Node &atual);
void printResultado(int quant_estados_espandidos, double total_tempo_programa, Matrix &sudoku_board);
bool insercaoEhValida(int possivel_numero, int pos_x, int pos_y, Matrix &sudoku_board, Node *no_parente);

// comparador para ordenar elementos
struct level_compare {
    // bool operator()(const Node* a, const Node* b) const {
    //     bool a_first = false;
    //     if(a->getParent()->getNivel() < b->getParent()->getNivel()) {
    //        a_first = true;
    //     } else {
    //         if(a->getParent()->getNivel() == b->getParent()->getNivel()) {
    //             if(a->getNivel() > b->getNivel()) {
    //                 a_first = true;
    //             }
    //         }
    //     }
    //     return a_first;
    // }
    bool operator()(const Node* a, const Node* b) const {
        if(a->getParent()->getNivel() < b->getParent()->getNivel() && a->getNivel() < b->getNivel()) return false;
        return true;
    }
};

// metodo auxiliar que printa uma matriz
void printMatrix(Matrix &matriz, string mensagem) {
	cout<<mensagem<<endl;
	for(size_t i = 0; i < matriz.size(); i++) {
    	cout<<"{";
		for(size_t j = 0; j < matriz[0].size() - 1; j++) {
			cout<<matriz[i][j]<<", ";
		}
		cout<<matriz[i][matriz[0].size()-1]<<"}"<<endl;
    }
    cout<<"\n"<<endl;
} // fim do metodo printMatrix

// metodo que printa o resultado no formato pedido
void printResultado(int quant_estados_espandidos, double total_tempo_programa, Matrix &sudoku_board) {
    cout << quant_estados_espandidos << " " << (int) total_tempo_programa << endl;
    for(int i = 0; i < QUANT_LINHAS; i++) {
        for(int j = 0; j < QUANT_COLUNAS; j++) {
            cout << sudoku_board[i][j];
        }
        cout << " ";
    }
    cout << endl;
    printMatrix(sudoku_board, "\nSudoku Resolvido! - Matriz final: ");
} // fim do metodo printResultado

// metodo auxiliar que printa um vetor
void printVetor(vector<int> &vetor, string mensagem, bool inline_print) {
	cout<<mensagem;
    if(!inline_print) cout<<endl;
    cout<<"{";
	for(size_t i = 0; i < vetor.size(); i++) {
		cout<<vetor[i];
        if(i < vetor.size() - 1) cout<<", ";
    }
	cout<<"}"<<endl;
    if(!inline_print) cout<<"\n"<<endl;
} // fim do metodo printVetor

// metodo auxiliar que printa uma mensagem
void testPrint(string mensagem) {
    if(TEST_PRINT) {
        cout<<mensagem << endl;
    }
} // fim do metodo testPrint

// metodo pra printar fila de nao expandidos
void printNaoExpandidosBFS(const queue<Node*>& queue_src, string mensagem) {
    bool debug = false;
    if(!queue_src.empty()) {
        if(debug) cout<<mensagem<<" Fila de nos nao expandidos:";
        queue<Node*> tempQueue = queue_src;
        while (!tempQueue.empty()) { // enquanto nao vazia
            cout<<tempQueue.front()->getWeight() << "["<<tempQueue.front()->getIPosition()<<"]["<<tempQueue.front()->getJPosition()<<"] - "; // pega o primeiro e printa 
            tempQueue.pop(); // remove o primeiro
        }
        if(debug) cout<<endl;
    } else {
        if(debug) cout<<"Fila atualmente esta vazia!"<<endl;
    }
} // fim do metodo printNaoExpandidosBFS


// metodo pra printar fila ordenada ids de nao expandidos
void printNaoExpandidosIDS(deque<Node*> queue_src, string mensagem) {
    bool debug = false;
    int i = 0;
    if(!queue_src.empty()) {
        if(debug) cout<<mensagem<<" Fila de nos nao expandidos:";
        deque<Node*> tempQueue = queue_src;
        while (!tempQueue.empty()) { // enquanto nao vazia
            if(debug && i > 0) cout << " - ";
            i++;
            if(debug) cout<<tempQueue.front()->getWeight() << "["<<tempQueue.front()->getIPosition()<<"]["<<tempQueue.front()->getJPosition()<<"]_nivel("<<tempQueue.front()->getNivel()<<")"; // pega o primeiro e printa 
            tempQueue.pop_front(); // remove o primeiro
        }
        if(debug) cout<<endl;
    } else {
        if(debug) cout<<"Fila atualmente esta vazia!"<<endl;
    }
} // fim do metodo printNaoExpandidosIDS

void printCaminhoProNo(Node *no_parente) {
    Node* new_parent_debug = no_parente;
    while(new_parent_debug->getWeight() != -1) {
        cout<<"parent is " << new_parent_debug->getWeight() << "["<<new_parent_debug->getIPosition()<<"]["<<new_parent_debug->getJPosition()<<"]" << endl;
        new_parent_debug = new_parent_debug->getParent();
    }
} // fim do metodo printCaminhoProNo

// metodo que preenche matriz resultado
void preencherMatrixResultado3(Matrix &sudoku_board, Node *no_corrente) {
    vector<Node*> caminho;
    while (no_corrente != nullptr) {
        caminho.push_back(no_corrente);
        no_corrente = no_corrente->getParent();
    }
    reverse(caminho.begin(), caminho.end());
    for (Node *no : caminho) {
        if (no->getWeight() != -1) {
            sudoku_board[no->getIPosition()][no->getJPosition()] = no->getWeight();
        }
    }
} // fim do metodo preencherMatrixResultado3

// metodo que preenche matriz resultado
void preencherMatrixResultado2(Matrix &sudoku_board, Node *no_corrente) {
    if (no_corrente == nullptr) {
        return;
    }
    if (no_corrente->getWeight() != -1) {
        sudoku_board[no_corrente->getIPosition()][no_corrente->getJPosition()] = no_corrente->getWeight();
    }
    for (Node *filho : no_corrente->getChildren()) {
        preencherMatrixResultado2(sudoku_board, filho);
    }
} // fim do metodo preencherMatrixResultado2

// metodo que preenche matriz resultado
void preencherMatrixResultado1(Matrix &sudoku_board, Node *no_corrente) {
    Node* atual = no_corrente;
    while(atual != nullptr) {
        if(atual->getWeight() != -1) {
            sudoku_board[atual->getIPosition()][atual->getJPosition()] = atual->getWeight();
        }
        atual = atual->getParent();
    }
} // fim do metodo preencherMatrixResultado1

// verifica menor entre dois ints
int menor(int a, int b) {
    if(b > a) return a;
    return b;
} // fim do metodo menor

// verifica maior entre dois ints
int maior(int a, int b) {
    if(b > a) return b;
    return a;
} // fim do metodo maior

// verifica se estao na mesma subdivisao do sudoku
bool mesmaSubdivisao(int x1, int y1, int x2, int y2) {
    int intervalo_x1_start = x1 - (x1 % TAMANHO_SUBDVISOES_LINHAS);
    int intervalo_x1_end = intervalo_x1_start + TAMANHO_SUBDVISOES_LINHAS - 1;
    int intervalo_y1_start = y1 - (y1 % TAMANHO_SUBDVISOES_COLUNAS);
    int intervalo_y1_end = intervalo_y1_start + TAMANHO_SUBDVISOES_COLUNAS - 1;
    if((x2 >= intervalo_x1_start) && (x2 <= intervalo_x1_end) && (y2 >= intervalo_y1_start) && (y2 <= intervalo_y1_end)) {
        return true;
    }
    return false;
} // fim do metodo mesmaSubdivisao

// metodo que libera memoria
void liberarMemoria(Node* no) {
    if (no == NULL) {
        return;
    }
    for (Node* filho : no->getChildren()) {
        liberarMemoria(filho);
    }
    delete no;
} // fim do metodo que liberarMemoria

// metodo que checa se uma possivel insercao eh valida
bool insercaoEhValida(int possivel_numero, int pos_x, int pos_y, Matrix &sudoku_board, Node *no_parente) {
    bool debug_insercao_valida = false;
    if(possivel_numero == 0) { // checando se insercao deste numero eh valida de acordo com numeros possiveis iniciais
        return false;
    } else {
        if(sudoku_board[pos_x][pos_y] != 0) { // nao posso inserir em local que ja possui numero
            return false;
        } else {
            while(no_parente->getWeight() != -1) {
                if(mesmaSubdivisao(no_parente->getIPosition(), no_parente->getJPosition(), pos_x, pos_y)) { // se na mesma divisao nao pode ter numeros repetidos
                    if(no_parente->getWeight() == possivel_numero) {
                        return false;
                    }
                } else {
                    if(no_parente->getIPosition() == pos_x || no_parente->getJPosition() == pos_y) { // se nao esta na mesma divisao porem esta na mesma linha ou coluna nao pode ter numeros repetidos 
                        if(no_parente->getWeight() == possivel_numero) {
                            return false;
                        }
                    } 
                }
                no_parente = no_parente->getParent();
            }
        }
    }
    return true;
} // fim do metodo que insercaoEhValida

// metodo que expande um no respeitando regras do sudoku
bool expandirNoBFS(Node* &no_corrente, queue<Node*> &nao_expandidos, Matrix &sudoku_board, vector<pair<int, int>> &vec_posicoes_vazias, Matrix &numeros_possiveis) {
    int no_corrente_indice = no_corrente->getNivel(); // pegando indice do no corrente
    bool debug = false;
    bool posso_inserir_numero = true;
    int possivel_numero = -999;
    int pos_x = -999;
    int pos_y = -999;
    bool um_ou_mais_nos_criados = false;
    for(int j = 0; j < QUANT_COLUNAS; j++) { // para cada possivel numero que podemos inserir nesse no
        possivel_numero = numeros_possiveis[no_corrente_indice][j];
        pos_x = vec_posicoes_vazias[no_corrente_indice].first;
        pos_y = vec_posicoes_vazias[no_corrente_indice].second;
        posso_inserir_numero = insercaoEhValida(possivel_numero, pos_x, pos_y, sudoku_board, no_corrente);
        if(posso_inserir_numero) {
            Node* no_filho = new Node(no_corrente, no_corrente->getNivel() + 1, pos_x, pos_y, possivel_numero);
            no_corrente->addChild(no_filho);
            nao_expandidos.push(no_filho);
            if(debug) printNaoExpandidosBFS(nao_expandidos, "IDS - ");
            um_ou_mais_nos_criados = true;
        } 
    }
    return um_ou_mais_nos_criados;
} // fim do metodo expandirNoBFS

// metodo que performa a busca em largura 
int breadth_first_search(Matrix &sudoku_board, vector<pair<int, int>> &vec_posicoes_vazias, int quant_posicoes_vazias, Matrix &numeros_possiveis) {

    // criando fila de nos nao expandidos
    queue<Node*> nao_expandidos;

    // criando no raiz
    Node* raiz = new Node(NULL, 0, vec_posicoes_vazias[0].first, vec_posicoes_vazias[0].second, -1);

    // colocando raiz na fronteira de nao expandidos
    nao_expandidos.push(raiz);

    Node* no_corrente;
    bool sucesso_bfs = false;
    int quant_nos_nivel = 1;
    int quant_estados_expandidos = 0;

    while(!nao_expandidos.empty()) {

        // pegando primeiro no nao explorado
        no_corrente = nao_expandidos.front(); 

        // removendo no expandido
        nao_expandidos.pop(); 
        
        // se ja atravessamos todas as posicoes vazias e nao sobrou nada
        if(no_corrente->getNivel() == quant_posicoes_vazias) { 
            sucesso_bfs = true; // sudoku resolvido, imprimir solução
            break;
        }

        // expandindo no
        expandirNoBFS(no_corrente, nao_expandidos, sudoku_board, vec_posicoes_vazias, numeros_possiveis);

        // incrementando contagem de estados expandidos
        quant_estados_expandidos++;

    }

    if(sucesso_bfs) {
        preencherMatrixResultado1(sudoku_board, no_corrente); // PARECE QUE PRINTA CORRETAMENTE
    } else {
        cout<<"Fracasso na BFS! Sudoku não resolvido" << endl;
    }

    return quant_estados_expandidos;
 
} // fim do metodo breadth_first_search

// metodo que expande um no respeitando regras do sudoku
bool expandirNoDFS(Node* &no_corrente, deque<Node*> &nao_expandidos, Matrix &sudoku_board, vector<pair<int, int>> &vec_posicoes_vazias, Matrix &numeros_possiveis, int &menor_nivel_atual) {
    bool debug = false;
    int no_corrente_indice = no_corrente->getNivel(); // pegando indice do no corrente
    bool posso_inserir_numero = true;
    int possivel_numero = -999;
    int pos_x = -999;
    int pos_y = -999;
    bool um_ou_mais_nos_criados = false;
    for(int j = 0; j < QUANT_COLUNAS; j++) { // para cada possivel numero que podemos inserir nesse no
        possivel_numero = numeros_possiveis[no_corrente_indice][j];
        pos_x = vec_posicoes_vazias[no_corrente_indice].first;
        pos_y = vec_posicoes_vazias[no_corrente_indice].second;
        posso_inserir_numero = insercaoEhValida(possivel_numero, pos_x, pos_y, sudoku_board, no_corrente);
        if(posso_inserir_numero) {
            Node* no_filho = new Node(no_corrente, no_corrente->getNivel() + 1, pos_x, pos_y, possivel_numero);
            no_corrente->addChild(no_filho);
            nao_expandidos.push_back(no_filho);
            if(debug) printNaoExpandidosIDS(nao_expandidos, "IDS - ");
            um_ou_mais_nos_criados = true;
        } 
    }
    return um_ou_mais_nos_criados;
} // fim do metodo expandirNoDFS

// metodo que performa a busca em profundidade 
int iterative_deepening_seach(Matrix &sudoku_board, vector<pair<int, int>> &vec_posicoes_vazias, int quant_posicoes_vazias, Matrix &numeros_possiveis) {

    // pra fins de debug
    bool debug = false;

    // criando deque de nos nao expandidos
    deque<Node*> nao_expandidos;

    // criando no raiz
    Node* raiz = new Node(NULL, 0, vec_posicoes_vazias[0].first, vec_posicoes_vazias[0].second, -1);

    // colocando raiz na fronteira de nao expandidos
    nao_expandidos.push_back(raiz);

    Node* no_corrente;
    bool sucesso_dfs = false;
    bool um_ou_mais_nos_criados = false;
    int quant_nos_nivel = 1;
    int quant_estados_expandidos = 0;
    int profundidade_aceita = 1; // controla a profundidade da expansao em cada passo
    int tmp_profundidade = profundidade_aceita; // controla profundidade dentro do loop
    bool explorando_em_profundidade = false; // se estamos explorando em profundidade
    int menor_nivel_atual = 0; // indica nivel do no mais antigo a ser adicionado na fila de nao expandidos

    while(!nao_expandidos.empty()) {
   
        if(explorando_em_profundidade && tmp_profundidade == 0) {
            explorando_em_profundidade = false;
            tmp_profundidade = profundidade_aceita;
        } 

        // terminei de explorar um nivel por inteiro
        if(!explorando_em_profundidade && nao_expandidos.front()->getNivel() > menor_nivel_atual) {
            explorando_em_profundidade = true;
            menor_nivel_atual++;
            profundidade_aceita++;
            tmp_profundidade = profundidade_aceita;
        } else {
            if(debug && explorando_em_profundidade) cout<<"Nao terminei de explorar nivel inteiro pois estamos explorando em largura"<<endl;
            if(debug && nao_expandidos.front()->getNivel() <= menor_nivel_atual) cout<<"Nao terminei de explorar nivel inteiro pois " << nao_expandidos.front()->getWeight() << "["<<nao_expandidos.front()->getIPosition()<<"]["<<nao_expandidos.front()->getJPosition()<<"]_nivel("<<nao_expandidos.front()->getNivel()<<") eh menor ou igual que menor_nivel_atual(" << menor_nivel_atual << ")" << endl;
        }

        // pegando primeiro no nao explorado
        if(explorando_em_profundidade) {
            no_corrente = nao_expandidos.back(); // pegando primeiro no em profundidade
            nao_expandidos.pop_back(); // removendo no expandido
        } else {
            no_corrente = nao_expandidos.front(); // pegando primeiro no em largura
            nao_expandidos.pop_front(); // removendo no expandido

        }

        // se ja atravessamos todas as posicoes vazias e nao sobrou nada
        if(no_corrente->getNivel() == quant_posicoes_vazias) { 
            sucesso_dfs = true; // sudoku resolvido, imprimir solução
            break;
        }

        // expandindo no e inserindo novos nos na lista de nao expandidos
        um_ou_mais_nos_criados = expandirNoDFS(no_corrente, nao_expandidos, sudoku_board, vec_posicoes_vazias, numeros_possiveis, menor_nivel_atual);

        // incrementando contagem de estados expandidos
        quant_estados_expandidos++;

        tmp_profundidade--;

    }

    if(sucesso_dfs) {
        preencherMatrixResultado1(sudoku_board, no_corrente); 
    } else {
        cout<<"Fracasso na DFS! Sudoku não resolvido" << endl;
    }

    return quant_estados_expandidos;
 
} // fim do metodo iterative_deepening_seach

// metodo que printa matrizes e vetores inseridos apos etapa de parsing da entrada
void verifica_insercao_correta(Matrix &sudoku_board,  Matrix &linha_numeros_possiveis, Matrix &coluna_numeros_possiveis, Matrix &numeros_possiveis) {
    printMatrix(sudoku_board, "Matriz sudoku_board:");
    printMatrix(linha_numeros_possiveis, "Matriz linha_numeros_possiveis:");
    printMatrix(coluna_numeros_possiveis, "Matriz coluna_numeros_possiveis:");
    printMatrix(numeros_possiveis, "Matriz numeros_possiveis Final:");
} // fim do metodo verifica_insercao_correta

// metodo que inicia o programa
int main(int argc, char **argv) {
    
    // invalidando algumas entradas ruins
    if((argc != (2+QUANT_LINHAS) && argc != 3) || (argc == 3 && (strlen(argv[2]) != (QUANT_LINHAS * QUANT_COLUNAS)))) {
        cout<<"Uso incorreto dos parametros de entrada!"<<endl;
        cout<<"Uso correto: TP1 [inicial do algoritmo de pesquisa] [configuracao sudoku em " << (QUANT_LINHAS * QUANT_COLUNAS) << " numeros separados por espaço a cada " << QUANT_LINHAS << " numeros]" << endl; 
        return 1;
    } else {
        if(argc == (2+QUANT_LINHAS)) {
            for(int i = 2; i < argc; i++) { 
                if(strlen(argv[i]) != QUANT_COLUNAS) {
                    cout<<"Uso incorreto dos parametros de entrada!"<<endl;
                    cout<<"Uso correto: TP1 [inicial do algoritmo de pesquisa] [configuracao sudoku em " << (QUANT_LINHAS * QUANT_COLUNAS) << " numeros separados por espaço a cada " << QUANT_LINHAS << " numeros] e " << argv[i] << " possi apenas " << strlen(argv[i]) << " numeros!" << endl; 
                    return 1;
                }
            }
        }
    }

    // criando matrizes de interesse
    Matrix sudoku_board(QUANT_LINHAS, vector<int>(QUANT_COLUNAS, 0));
    Matrix linha_numeros_possiveis(QUANT_LINHAS, vector<int>(QUANT_COLUNAS, 0));
    Matrix coluna_numeros_possiveis(QUANT_LINHAS, vector<int>(QUANT_COLUNAS, 0));

    // criando lista de locais vazios para travessia facilitada e contador de posicoes vazias
    vector<pair<int, int>> vec_posicoes_vazias;
    int quant_posicoes_vazias = 0;

    // preenchendo matrizes com possiveis numeros para espacos vazios
    // os numeros abaixo sao todos os de 1 a 9 pois nao ha nenhum numero do sudoku inserido ate entao
    for(int i = 0; i < QUANT_LINHAS; i++) {
        for(int j = 0; j < QUANT_COLUNAS; j++) {
            linha_numeros_possiveis[i][j] = j+1;
            coluna_numeros_possiveis[i][j] = j+1;
        }
    }
    
    // inserindo matriz do sudoku lido e restringindo opcoes de numeros possiveis
    int k = 0;
    for(int i = 0; i < QUANT_LINHAS; i++) {
        for(int j = 0; j < QUANT_COLUNAS; j++) {
            if(argc == 3) {
                sudoku_board[i][j] = int(string(argv[2]).at(k)) - 48;
                k++;
            } else {
                sudoku_board[i][j] = int(string(argv[2 + i]).at(j)) - 48;
            }
            linha_numeros_possiveis[i][sudoku_board[i][j] - 1] = 0;
            coluna_numeros_possiveis[j][sudoku_board[i][j] - 1] = 0;
            if(sudoku_board[i][j] == 0) {
                vec_posicoes_vazias.push_back(make_pair(i, j)); // inserindo posicao vazia que temos que usar
                quant_posicoes_vazias++;
            }
        }
    }    

    // excluindo numeros a partir das linhas e colunas
    Matrix numeros_possiveis(quant_posicoes_vazias, vector<int>(QUANT_COLUNAS, 0));
    for(int i = 0; i < quant_posicoes_vazias; i++) {
        for(int j = 0; j < QUANT_COLUNAS; j++) {
            numeros_possiveis[i][j] = linha_numeros_possiveis[vec_posicoes_vazias[i].first][j];
            if(coluna_numeros_possiveis[vec_posicoes_vazias[i].second][j] == 0) {
                numeros_possiveis[i][j] = 0;
            }
        }
    }

    // excluindo numeros a partir das subdivisoes
    for(int k = 0; k < quant_posicoes_vazias; k++) {
        int i_start_pos = vec_posicoes_vazias[k].first - (vec_posicoes_vazias[k].first % TAMANHO_SUBDVISOES_LINHAS);
        int i_end_pos = i_start_pos + TAMANHO_SUBDVISOES_LINHAS - 1;
        int j_start_pos = vec_posicoes_vazias[k].second - (vec_posicoes_vazias[k].second  % TAMANHO_SUBDVISOES_COLUNAS);
        int j_end_pos = j_start_pos + TAMANHO_SUBDVISOES_COLUNAS - 1;
        for(int i = i_start_pos; i <= i_end_pos; i++) {
            for(int j = j_start_pos; j <= j_end_pos; j++) {
                if(sudoku_board[i][j] != 0) {
                    numeros_possiveis[k][sudoku_board[i][j] - 1] = 0;
                }
            }
        }
    }

    // printando matrizes para verificar insercao
    verifica_insercao_correta(sudoku_board, linha_numeros_possiveis, coluna_numeros_possiveis, numeros_possiveis);

    // executando algoritmos de busca e printando resultado final
    int quant_estados_espandidos = 0;
    auto inicio_tempo_programa = std::chrono::high_resolution_clock::now(); // inicio da contagem do tempo
    switch(argv[1][0])  {
        case 'B':
            cout<< "Selected: breadth_first_search\n" << endl;
            quant_estados_espandidos = breadth_first_search(sudoku_board, vec_posicoes_vazias, quant_posicoes_vazias, numeros_possiveis); // TERMINA - SOLUCAO ERRADA
            break;
        case 'I':
            cout<< "Selected: iterative_deepening_seach\n" << endl;
            quant_estados_espandidos = iterative_deepening_seach(sudoku_board, vec_posicoes_vazias, quant_posicoes_vazias, numeros_possiveis); // TERMINA - SOLUCAO ERRADA
            break;
        default:
            cout<<"Algoritmo invalido. Escolha entre: Breadth-first search e Iterative deepening search" << endl;
    }
    auto fim_tempo_programa = std::chrono::high_resolution_clock::now(); // fim da contagem do tempo
    std::chrono::duration<double,std::milli> total_tempo_programa = fim_tempo_programa - inicio_tempo_programa; // calculo do tempo decorrido em milissegundos
    cout<<"Count is: "<<total_tempo_programa.count()<<endl;
    printResultado(quant_estados_espandidos, total_tempo_programa.count(), sudoku_board);

    return 0;

} // fim da main