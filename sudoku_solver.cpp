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

class Node {
    private:
        Node* parent_node;
        int nivel;
        std::list<Node*> child_nodes;
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
        const std::list<Node*>& getChildren() const {
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

            std::stringstream ss;

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
void printNaoExpandidosBFS(const std::queue<Node*>& queue_src, string mensagem) {
    bool debug = false;
    if(!queue_src.empty()) {
        if(debug) cout<<mensagem<<" Fila de nos nao expandidos:";
        std::queue<Node*> tempQueue = queue_src;
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
void printNaoExpandidosIDS(std::deque<Node*> queue_src, string mensagem) {
    bool debug = false;
    int i = 0;
    if(!queue_src.empty()) {
        if(debug) cout<<mensagem<<" Fila de nos nao expandidos:";
        std::deque<Node*> tempQueue = queue_src;
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

// metodo que printa caminho do no ate a raiz
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
    std::reverse(caminho.begin(), caminho.end());
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

// metodo que verifica se insercao eh valida baseada na posicao
bool insercaoEhValidaNaPosicao(int possivel_numero, int pos_x, int pos_y, const Matrix &sudoku_board) {
    if(possivel_numero == 0 || sudoku_board[pos_x][pos_y] != 0) {
        return false;
    }
    return true;
}

// metodo que verifica se insercao eh valida baseado nos parentes ja inseridos
bool insercaoEhValidaPorParentes(int possivel_numero, int pos_x, int pos_y, const Matrix &sudoku_board, const Node* no_parente) {
    while(no_parente->getWeight() != -1) {
        if(mesmaSubdivisao(no_parente->getIPosition(), no_parente->getJPosition(), pos_x, pos_y) ||
           no_parente->getIPosition() == pos_x || no_parente->getJPosition() == pos_y) {
            if(no_parente->getWeight() == possivel_numero) {
                return false;
            }
        }
        no_parente = no_parente->getParent();
    }
    return true;
}

// metodo que checa se uma possivel insercao eh valida
bool insercaoEhValida(int possivel_numero, int pos_x, int pos_y, Matrix &sudoku_board, Node *no_parente) {
    if(!insercaoEhValidaNaPosicao(possivel_numero, pos_x, pos_y, sudoku_board)) {
        return false;
    }
    if(!insercaoEhValidaPorParentes(possivel_numero, pos_x, pos_y, sudoku_board, no_parente)) {
        return false;
    }
    return true;
}

// metodo que gera nos filhos
vector<Node*> generateChildNodes(Node* no_corrente, Matrix &sudoku_board, const vector<pair<int, int>> &vec_posicoes_vazias, const Matrix &numeros_possiveis) {
    vector<Node*> child_nodes;
    int no_corrente_indice = no_corrente->getNivel();
    int pos_x = vec_posicoes_vazias[no_corrente_indice].first;
    int pos_y = vec_posicoes_vazias[no_corrente_indice].second;

    for(int j = 0; j < QUANT_COLUNAS; j++) {
        int possivel_numero = numeros_possiveis[no_corrente_indice][j];
        if(insercaoEhValida(possivel_numero, pos_x, pos_y, sudoku_board, no_corrente)) {
            Node* no_filho = new Node(no_corrente, no_corrente->getNivel() + 1, pos_x, pos_y, possivel_numero);
            no_corrente->addChild(no_filho);
            child_nodes.push_back(no_filho);
        }
    }
    return child_nodes;
}

// metodo que expande um no durante a BFS respeitando regras do sudoku
bool expandirNoBFS(Node* &no_corrente, std::queue<Node*> &nao_expandidos, Matrix &sudoku_board, vector<pair<int, int>> &vec_posicoes_vazias, Matrix &numeros_possiveis) {
    vector<Node*> child_nodes = generateChildNodes(no_corrente, sudoku_board, vec_posicoes_vazias, numeros_possiveis);
    for (Node* child : child_nodes) {
        nao_expandidos.push(child);
    }
    return !child_nodes.empty();
}

// metodo que expande um no durante a IDS respeitando regras do sudoku
bool expandirNoIDS(Node* &no_corrente, std::deque<Node*> &nao_expandidos, Matrix &sudoku_board, vector<pair<int, int>> &vec_posicoes_vazias, Matrix &numeros_possiveis, int &menor_nivel_atual) {
    vector<Node*> child_nodes = generateChildNodes(no_corrente, sudoku_board, vec_posicoes_vazias, numeros_possiveis);
    for (Node* child : child_nodes) {
        nao_expandidos.push_back(child);
    }
    return !child_nodes.empty();
}

// metodo que performa a busca em largura 
int breadth_first_search(Matrix &sudoku_board, vector<pair<int, int>> &vec_posicoes_vazias, int quant_posicoes_vazias, Matrix &numeros_possiveis) {

    // criando fila de nos nao expandidos
    std::queue<Node*> nao_expandidos;

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

// metodo que performa a busca em profundidade 
int iterative_deepening_seach(Matrix &sudoku_board, vector<pair<int, int>> &vec_posicoes_vazias, int quant_posicoes_vazias, Matrix &numeros_possiveis) {

    // pra fins de debug
    bool debug = false;

    // criando std::deque de nos nao expandidos
    std::deque<Node*> nao_expandidos;

    // criando no raiz
    Node* raiz = new Node(NULL, 0, vec_posicoes_vazias[0].first, vec_posicoes_vazias[0].second, -1);

    // colocando raiz na fronteira de nao expandidos
    nao_expandidos.push_back(raiz);

    Node* no_corrente;
    bool sucesso_ids = false;
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
            sucesso_ids = true; // sudoku resolvido, imprimir solução
            break;
        }

        // expandindo no e inserindo novos nos na lista de nao expandidos
        um_ou_mais_nos_criados = expandirNoIDS(no_corrente, nao_expandidos, sudoku_board, vec_posicoes_vazias, numeros_possiveis, menor_nivel_atual);

        // incrementando contagem de estados expandidos
        quant_estados_expandidos++;

        tmp_profundidade--;

    }

    if(sucesso_ids) {
        preencherMatrixResultado1(sudoku_board, no_corrente); 
    } else {
        cout<<"Fracasso na IDS! Sudoku não resolvido" << endl;
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

// metodo que faz a validacao dos parametros de entrada do programa
bool validaParametrosEntrada(int argc, char **argv) {
    if((argc != (2+QUANT_LINHAS) && argc != 3) || (argc == 3 && (strlen(argv[2]) != (QUANT_LINHAS * QUANT_COLUNAS)))) {
        cout<<"Uso incorreto dos parametros de entrada!"<<endl;
        cout<<"Uso correto: TP1 [inicial do algoritmo de pesquisa] [configuracao sudoku em " << (QUANT_LINHAS * QUANT_COLUNAS) << " numeros separados por espaço a cada " << QUANT_LINHAS << " numeros]" << endl; 
        return false;
    } else {
        if(argc == (2+QUANT_LINHAS)) {
            for(int i = 2; i < argc; i++) { 
                if(strlen(argv[i]) != QUANT_COLUNAS) {
                    cout<<"Uso incorreto dos parametros de entrada!"<<endl;
                    cout<<"Uso correto: TP1 [inicial do algoritmo de pesquisa] [configuracao sudoku em " << (QUANT_LINHAS * QUANT_COLUNAS) << " numeros separados por espaço a cada " << QUANT_LINHAS << " numeros] e " << argv[i] << " possi apenas " << strlen(argv[i]) << " numeros!" << endl; 
                    return false;
                }
            }
        }
    }
    return true;
} // fim do metodo validaParametrosEntrada

// metodo que faz a insercao dos valores do tabuleiro do sudoku na matriz correspondente
void inicializaTabuleiroSudoku(Matrix &sudoku_board, Matrix &linha_numeros_possiveis, Matrix &coluna_numeros_possiveis, vector<pair<int, int>> &vec_posicoes_vazias, int &quant_posicoes_vazias, int argc, char **argv) {
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
                vec_posicoes_vazias.push_back(std::make_pair(i, j)); // inserindo posicao vazia que temos que usar
                quant_posicoes_vazias++;
            }
        }
    }    
} // fim do metodo inicializaTabuleiroSudoku

// metodo que exclui possibilidades de numeros baseado em numeros ja existente nas mesmas linhas
void excluiNumerosBaseadoEmLinhas(Matrix &numeros_possiveis, const Matrix &linha_numeros_possiveis, const vector<pair<int, int>> &vec_posicoes_vazias) {
    for(int i = 0; i < vec_posicoes_vazias.size(); i++) {
        for(int j = 0; j < QUANT_COLUNAS; j++) {
            numeros_possiveis[i][j] = linha_numeros_possiveis[vec_posicoes_vazias[i].first][j];
        }
    }
} // fim do metodo excluiNumerosBaseadoEmLinhas

// metodo que exclui possibilidades de numeros baseado em numeros ja existente nas mesmas colunas
void excluiNumerosBaseadoEmColunas(Matrix &numeros_possiveis, const Matrix &coluna_numeros_possiveis, const vector<pair<int, int>> &vec_posicoes_vazias) {
    for(int i = 0; i < vec_posicoes_vazias.size(); i++) {
        for(int j = 0; j < QUANT_COLUNAS; j++) {
            if(coluna_numeros_possiveis[vec_posicoes_vazias[i].second][j] == 0) {
                numeros_possiveis[i][j] = 0;
            }
        }
    }
} // fim do metodo excluiNumerosBaseadoEmColunas

// metodo que exclui possibilidades de numeros baseado em numeros ja existente nas mesmas subdivisoes
void excluiNumerosBaseadoEmSubdivisoes(Matrix &numeros_possiveis, const Matrix &sudoku_board, const vector<pair<int, int>> &vec_posicoes_vazias) {
    for(int k = 0; k < vec_posicoes_vazias.size(); k++) {
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
} // fim do metodo excluiNumerosBaseadoEmSubdivisoes

// metodo que inicia o programa
int main(int argc, char **argv) {

    if (!validaParametrosEntrada(argc, argv)) {
        return 1;
    }

    Matrix sudoku_board(QUANT_LINHAS, vector<int>(QUANT_COLUNAS, 0));
    Matrix linha_numeros_possiveis(QUANT_LINHAS, vector<int>(QUANT_COLUNAS, 0));
    Matrix coluna_numeros_possiveis(QUANT_LINHAS, vector<int>(QUANT_COLUNAS, 0));
    vector<pair<int, int>> vec_posicoes_vazias;
    int quant_posicoes_vazias = 0;

    for(int i = 0; i < QUANT_LINHAS; i++) {
        for(int j = 0; j < QUANT_COLUNAS; j++) {
            linha_numeros_possiveis[i][j] = j+1;
            coluna_numeros_possiveis[i][j] = j+1;
        }
    }

    inicializaTabuleiroSudoku(sudoku_board, linha_numeros_possiveis, coluna_numeros_possiveis, vec_posicoes_vazias, quant_posicoes_vazias, argc, argv);

    Matrix numeros_possiveis(quant_posicoes_vazias, vector<int>(QUANT_COLUNAS, 0));
    excluiNumerosBaseadoEmLinhas(numeros_possiveis, linha_numeros_possiveis, vec_posicoes_vazias);
    excluiNumerosBaseadoEmColunas(numeros_possiveis, coluna_numeros_possiveis, vec_posicoes_vazias);
    excluiNumerosBaseadoEmSubdivisoes(numeros_possiveis, sudoku_board, vec_posicoes_vazias);

    verifica_insercao_correta(sudoku_board, linha_numeros_possiveis, coluna_numeros_possiveis, numeros_possiveis);

    int quant_estados_espandidos = 0;
    auto inicio_tempo_programa = std::chrono::high_resolution_clock::now(); 
    switch(argv[1][0])  {
        case 'B':
            cout<< "Selected: breadth_first_search\n" << endl;
            quant_estados_espandidos = breadth_first_search(sudoku_board, vec_posicoes_vazias, quant_posicoes_vazias, numeros_possiveis); 
            break;
        case 'I':
            cout<< "Selected: iterative_deepening_seach\n" << endl;
            quant_estados_espandidos = iterative_deepening_seach(sudoku_board, vec_posicoes_vazias, quant_posicoes_vazias, numeros_possiveis); 
            break;
        default:
            cout<<"Algoritmo invalido. Escolha entre: Breadth-first search e Iterative deepening search" << endl;
    }
    auto fim_tempo_programa = std::chrono::high_resolution_clock::now(); 
    std::chrono::duration<double,std::milli> total_tempo_programa = fim_tempo_programa - inicio_tempo_programa; 
    cout<<"Count is: "<<total_tempo_programa.count()<<endl;
    printResultado(quant_estados_espandidos, total_tempo_programa.count(), sudoku_board);

    return 0;
} // fim do metodo main