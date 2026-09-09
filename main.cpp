#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <vector>

using namespace std;

//struct para armazenar as informacoes da imagem
struct ImagemInterna{
    int largura;
    int altura;
    vector<unsigned char> pixels; //R, G, B de cada pixel
};

string pegarValor(string json, string nome) {
    string search = "\"" + nome + "\":";

    int inicio = json.find(search);
    if (inicio == -1) {
        return "Nome não encontrado";
    }

    inicio = inicio+search.length();
    if (json[inicio] == '"') {
        inicio++;
        int fim = json.find('"', inicio);
        return json.substr(inicio, fim - inicio);
    }
}

//funcao para ler o json linha por linha do arquivo txt
string lerArquivoJson(string caminhoArquivo){
    ifstream arquivo(caminhoArquivo);

    if(!arquivo.is_open()){
        cerr << "Erro: arquivo nao encontrado" << endl;
    }

    string conteudo = "";
    string linha;

    //guarda linha por linha do arquivo em uma string
    while(getline(arquivo, linha)){
        conteudo += linha;
    }

    //fechar o arquivo depois de terminar a leitura 
    arquivo.close();

    return conteudo;
}


//funcao para recortar a imagem
ImagemInterna corta(ImagemInterna original, int x, int y, int novaLargura, int novaAltura)
{
    ImagemInterna recorte;
    recorte.largura = novaLargura;
    recorte.altura = novaAltura;

    //preencher o vetor de pixels com o tamanho correto
    recorte.pixels.resize(novaLargura * novaAltura * 3);

    //for para controlar a altura do recorte
    for (int linha = 0; linha < novaAltura; linha++)
    {
        //calcular o inicio da linha na imagem original e no recorte
        int inicioOriginal = ((y + linha) * original.largura + x) * 3;
        int inicioRecorte = linha * novaLargura * 3;

        //percorrer os bytes da linha e copiar para o vetor de pixels do recorte
        for (int i = 0; i < novaLargura * 3; i++)
            recorte.pixels[inicioRecorte + i] = original.pixels[inicioOriginal + i];
    }

    return recorte;
}

//Função para leitura de cores

int main()
{
    //setar caracteres em portugues
    setlocale(LC_ALL, "");

    //[INPUT]
    string json = lerArquivoJson("nome.txt");

    string nomeArquivo;
    cout << "Digite o nome do arquivo BMP: " << endl;
    getline(cin, nomeArquivo);

    int novaAltura, novaLargura;
    cout << "--Configuracoes de recorte--" << endl;
    cout << "Digite a nova altura da imagem: " << endl;
    cin >> novaAltura;

    cout << "Digite a nova largura da imagem: " << endl;
    cin >> novaLargura;

    //[PROCESS]

    //cria a imagem original
    ImagemInterna imagemOriginal;

    //cria a copia da imagem original
    ImagemInterna imagemCopia = imagemOriginal;

    //valores ficticios para teste
    ImagemInterna imagemRecortada = corta(imagemCopia, 10, 10, novaLargura, novaAltura);

    return 0;
}
