#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <vector>
#include <cstdint>
#include <algorithm>

using namespace std;

//formatação do cabecalho, nao permite espacos preenchidos por oq nao deve
#pragma pack(push, 1)

     //cabecalho do arquivo
struct CabecalhoBMP{
    uint16_t tipo; // BM
    uint32_t tamanhoArquivo;
    uint16_t reservado1;
    uint16_t reservado2;
    uint32_t offset; // inicio dos pixels de cor
};

    //cabecalho com infos da imagem.bmp
struct CabecalhoImagem{
    uint32_t tamanho; // tamanho desse cabecalho aqui
    int32_t largura;
    int32_t altura;
    uint16_t planos;
    uint16_t bitsPorPixel;
    uint32_t compressao;
    uint32_t tamanhoImagem;
    int32_t larguraPixel;
    int32_t alturaPixel;
    uint32_t cores;
    uint32_t coresImportantes;
};

#pragma pack(pop)

// struct para armazenar as informacoes da imagem
struct ImagemInterna{
    int largura;
    int altura;
    vector<unsigned char> pixels;
};

//funcao para converter a imagem para tons de cinza
ImagemInterna ConvGrey(ImagemInterna imagem)
{
    //loop para separa os bytes R G e B e inverte ja que o bmp fica B G R
    for (int i = 0; i < imagem.pixels.size(); i += 3)
    {
        unsigned char B = imagem.pixels[i];
        unsigned char G = imagem.pixels[i + 1];
        unsigned char R = imagem.pixels[i + 2];

        //Conversao por media ponderada em cinza
        unsigned char gray = 0.30 * R + 0.59 * G + 0.11 * B;

        imagem.pixels[i] = gray;
        imagem.pixels[i + 1] = gray;
        imagem.pixels[i + 2] = gray;
    }

    return imagem;
}

//funcao para pegar valores do arquivo json
string PegarValorJson(string json, string nome)
{
    //faz um "scanner" procurando só o nome no json
    string procurar = "\"" + nome + "\":";

    int inicio = json.find(procurar);

    //caso nao encontre o nome do comando
    if (inicio == -1)
        return "Nome nao encontrado";

    //separaçao de nome e valor dos jsons
    inicio += procurar.length();

    while (json[inicio] == ' ')
        inicio++;

    // String
    if (json[inicio] == '"')
    {
        inicio++;

        int fim = json.find('"', inicio);

        return json.substr(inicio, fim - inicio);
    }

    // Vetor [x, y]
    if (json[inicio] == '[')
    {
        int fim = json.find(']', inicio);

        return json.substr(inicio, fim - inicio + 1);
    }

    // Numero
    int fim = json.find_first_of(",}", inicio);

    return json.substr(inicio, fim - inicio);
}

//funcao para ler o arquivo json
string LerArquivoJson(string caminhoArquivo)
{
    ifstream arquivo(caminhoArquivo);

    if (!arquivo.is_open()) {
        cerr << "Erro: arquivo nao encontrado" << endl;
        exit(1);
    }
    string conteudo = "";
    string linha;

    //le o arquivo linha a linha
    while (getline(arquivo, linha)){
        conteudo += linha;
    }

    arquivo.close();

    return conteudo;
}

//funcao para recortar a imagem
ImagemInterna Corta(ImagemInterna original, int x, int y, int novaLargura, int novaAltura)
{
    ImagemInterna recorte;

    recorte.largura = novaLargura;
    recorte.altura = novaAltura;

    recorte.pixels.resize(novaLargura * novaAltura * 3);

    //troca de tamanho direto da struct do arquivo
    for (int linha = 0; linha < novaAltura; linha++)
    {
        int inicioOriginal =
            ((y + linha) * original.largura + x) * 3;

        int inicioRecorte =
            linha * novaLargura * 3;

        for (int i = 0; i < novaLargura * 3; i++)
        {
            recorte.pixels[inicioRecorte + i] =
                original.pixels[inicioOriginal + i];
        }
    }

    return recorte;
}

// funcao para converter um vetor json "[x, y]" em dois inteiros
void ParseVetor(string valor, int &a, int &b){
    valor.erase(remove(valor.begin(), valor.end(), '['), valor.end());
    valor.erase(remove(valor.begin(), valor.end(), ']'), valor.end());
    valor.erase(remove(valor.begin(), valor.end(), ' '), valor.end());

    int virgula = valor.find(',');

    a = stoi(valor.substr(0, virgula));
    b = stoi(valor.substr(virgula + 1));
}

//funcao para assumir bmp
string ColocarBMP(string nomeArquivo)
{
    //coloca .bmp se nao tiver no fim
    if (nomeArquivo.find(".bmp") == string::npos)
    {
        nomeArquivo += ".bmp";
    }

    return nomeArquivo;
}

//funcao para abrir a imagem
ImagemInterna AbrirImagem(string json, ImagemInterna imagemOriginal){

    //pegando nome da imagem
    string nomeArquivo = PegarValorJson(json, "arq1");

    //aplicando a funcao caso precise
    nomeArquivo = ColocarBMP(nomeArquivo);

    ifstream arquivoImagem;
    arquivoImagem.open(nomeArquivo, ios::in | ios::binary);

    if (!arquivoImagem.is_open()){
        cerr << "Erro: nao foi possivel abrir a imagem" << endl;
        exit(1);
    }

    // le o offset real dos dados de pixel (ultimos antes dos pixels)
   // Le o cabecalho do arquivo BMP
CabecalhoBMP cabecalhoArquivo;
CabecalhoImagem cabecalhoImagem;

// Volta para o inicio do arquivo
arquivoImagem.seekg(0);

// le o primeiro cabecalho (14 bytes)
arquivoImagem.read(
    (char*)&cabecalhoArquivo,
    sizeof(CabecalhoBMP)
);

// le o segundo cabecalho (40 bytes)
arquivoImagem.read(
    (char*)&cabecalhoImagem,
    sizeof(CabecalhoImagem)
);


// mostra as informacoes do cabecalho
cerr << endl;
cerr << "===== CABECALHO BMP =====" << endl;

cerr << "Tipo: ";


// O BMP guarda esses dois caracteres(B e M) dentro de um numero.
cerr << (char)(cabecalhoArquivo.tipo & 0xFF);
cerr << (char)(cabecalhoArquivo.tipo >> 8);

cerr << endl;

cerr << "Tamanho do arquivo: "
     << cabecalhoArquivo.tamanhoArquivo
     << " bytes" << endl;

cerr << "Inicio dos pixels: "
     << cabecalhoArquivo.offset
     << " bytes" << endl;

cerr << "Largura: "
     << cabecalhoImagem.largura
     << " pixels" << endl;

cerr << "Altura: "
     << cabecalhoImagem.altura
     << " pixels" << endl;

cerr << "Bits por pixel: "
     << cabecalhoImagem.bitsPorPixel
     << endl;

cerr << "Compressao: "
     << cabecalhoImagem.compressao
     << endl;

cerr << "Tamanho dos pixels: "
     << cabecalhoImagem.tamanhoImagem
     << " bytes" << endl;

cerr << "=========================" << endl;


// confere se o arquivo e realmente um BMP
if ((char)(cabecalhoArquivo.tipo & 0xFF) != 'B' ||
    (char)(cabecalhoArquivo.tipo >> 8) != 'M')
{
    cerr << "Erro: o arquivo nao e um BMP." << endl;
    exit(1);
}

// olha se a imagem tem 24 bits por pixel
if (cabecalhoImagem.bitsPorPixel != 24)
{
    cerr << "Erro: o BMP nao possui 24 bits por pixel." << endl;
    exit(1);
}


// 0 significa que nao existe compressao
if (cabecalhoImagem.compressao != 0)
{
    cerr << "Erro: o BMP possui compressao." << endl;
    exit(1);
}

cerr << "BMP conferido: formato valido para o trabalho." << endl;


// Pega largura e altura do cabecalho
int largura = cabecalhoImagem.largura;
int alturaBruta = cabecalhoImagem.altura;

// Se a altura for positiva, o bmp ta invertido
bool armazenadaDeBaixoParaCima = (alturaBruta > 0);

// valor absoluto
int altura = abs(alturaBruta);


// guarda as informacoes na ImagemInterna
imagemOriginal.largura = largura;
imagemOriginal.altura = altura;

imagemOriginal.pixels.resize(
    (size_t)largura * altura * 3
);


// Cada pixel possui 3 bytes ( R G e B)

int larguraBytes = largura * 3;


// O BMP precisa que cada linha tenha um tamanho multiplo de 4 byte, os extras são padding

int padding = (4 - (larguraBytes % 4)) % 4;


// pula para o local onde os pixels comecam
arquivoImagem.seekg(cabecalhoArquivo.offset);


// Le cada linha da imagem
for (int linha = 0; linha < altura; linha++)
{
    // se tiver invertida, coloca ela normal
    int linhaDestino;

    if (armazenadaDeBaixoParaCima)
        linhaDestino = altura - 1 - linha;
    else
        linhaDestino = linha;

    int inicio = linhaDestino * larguraBytes;

    // le os pixels da linha
    for (int i = 0; i < larguraBytes; i++)
    {
        imagemOriginal.pixels[inicio + i] =
            arquivoImagem.get();
    }

    // pula o padding no final da linha se tiver
    arquivoImagem.ignore(padding);
}
    arquivoImagem.close();

    return imagemOriginal;
}

//funcao para gravar a imagem
void GravaBMP(ImagemInterna imagem, string nomeArquivo){

    //funcao assumindo bmp caso necessario
    nomeArquivo = ColocarBMP(nomeArquivo);

    //escrever dados do arquivo BMP
    ofstream arquivoImagem(nomeArquivo, ios::out | ios::binary);

    if (!arquivoImagem.is_open()){
        cerr << "Erro: nao foi possivel criar o arquivo " << nomeArquivo << endl;
        exit(1);
    }

    int larguraBytes = imagem.largura * 3;
    // garantir que cada linha de pixels no arquivo BMP ocupe um número de bytes múltiplo de 4
    int padding = (4 - (larguraBytes % 4)) % 4;
    int tamanhoDados = (larguraBytes + padding) * imagem.altura;
    int tamanhoArquivo = 54 + tamanhoDados;

    // cabecalho inteiro (54 bytes) guardado num vetor, comecando zerado
    vector<unsigned char> cabecalho(54, 0);

    cabecalho[0] = 'B';
    cabecalho[1] = 'M';

    // tamanho do arquivo (posicoes 2 a 5)
    for (int i = 0; i < 4; i++){
        cabecalho[2 + i] = (tamanhoArquivo >> (i * 8)) & 0xFF;
    }

    // offset dos dados (posicoes 10 a 13)
    cabecalho[10] = 54;

    // tamanho do segundo cabecalho (posicoes 14 a 17)
    cabecalho[14] = 40;

    // largura (posicoes 18 a 21)
    for (int i = 0; i < 4; i++){
        cabecalho[18 + i] = (imagem.largura >> (i * 8)) & 0xFF;
    }

    // altura (posicoes 22 a 25)
    for (int i = 0; i < 4; i++){
        cabecalho[22 + i] = (imagem.altura >> (i * 8)) & 0xFF;
    }

    // planos (posicoes 26-27)
    cabecalho[26] = 1;

    // bits por pixel (posicoes 28-29)
    cabecalho[28] = 24;

    // tamanho da imagem em bytes (posicoes 34 a 37)
    for (int i = 0; i < 4; i++){
        cabecalho[34 + i] = (tamanhoDados >> (i * 8)) & 0xFF;
    }

    // (compressao, resolucoes e cores ficam 0, ja que o vetor comecou zerado)

    // escreve o cabecalho inteiro
    for (int i = 0; i < cabecalho.size(); i++)
        arquivoImagem.put(cabecalho[i]);

    // escreve os pixels
    //arruma por largura para nao inverter verticalmente
    for (int linha = imagem.altura - 1; linha >= 0; linha--)
    {
        for (int i = 0; i < larguraBytes; i++)
            arquivoImagem.put(imagem.pixels[linha * larguraBytes + i]);

        for (int i = 0; i < padding; i++)
            arquivoImagem.put(0);
    }

    arquivoImagem.close();

    cerr << "Arquivo " << nomeArquivo << " gravado com sucesso." << endl;
}

//funcao para pegar a imagem, nome e canal de cor e gravar em json
void GravaJson(ImagemInterna imagem, string nomeArquivo, char canal)
{
    // Se nao tiver extensao, coloca .jbmp
    if (nomeArquivo.find(".jbmp") == string::npos)
    {
        nomeArquivo += ".jbmp";
    }

    ofstream arquivo(nomeArquivo);

    if (!arquivo.is_open())
    {
        cerr << "Erro: nao foi possivel criar o arquivo "
             << nomeArquivo << endl;
        return;
    }

    // Se canal for A, grava os tres canais
    if (canal == 'A')
    {
        arquivo << "{" << endl;

        // Canal R
        arquivo << "\"R\": [" << endl;

        for (int y = 0; y < imagem.altura; y++)
        {
            arquivo << "[";

            for (int x = 0; x < imagem.largura; x++)
            {
                int posicao = (y * imagem.largura + x) * 3;

                // BMP guarda B G R
                int R = imagem.pixels[posicao + 2];

                arquivo << (int)R;

                if (x < imagem.largura - 1)
                    arquivo << ", ";
            }

            arquivo << "]";

            if (y < imagem.altura - 1)
                arquivo << ",";

            arquivo << endl;
        }

        arquivo << "]," << endl;

        // Canal G
        arquivo << "\"G\": [" << endl;

        for (int y = 0; y < imagem.altura; y++)
        {
            arquivo << "[";

            for (int x = 0; x < imagem.largura; x++)
            {
                int posicao = (y * imagem.largura + x) * 3;

                int G = imagem.pixels[posicao + 1];

                arquivo << (int)G;

                if (x < imagem.largura - 1)
                    arquivo << ", ";
            }

            arquivo << "]";

            if (y < imagem.altura - 1)
                arquivo << ",";

            arquivo << endl;
        }

        arquivo << "]," << endl;

        // Canal B
        arquivo << "\"B\": [" << endl;

        for (int y = 0; y < imagem.altura; y++)
        {
            arquivo << "[";

            for (int x = 0; x < imagem.largura; x++)
            {
                int posicao = (y * imagem.largura + x) * 3;

                int B = imagem.pixels[posicao];

                arquivo << (int)B;

                if (x < imagem.largura - 1)
                    arquivo << ", ";
            }

            arquivo << "]";

            if (y < imagem.altura - 1)
                arquivo << ",";

            arquivo << endl;
        }

        arquivo << "]" << endl;
        arquivo << "}" << endl;
    }
    else
    {
        // Grava somente um canal

        arquivo << "[" << endl;

        for (int y = 0; y < imagem.altura; y++)
        {
            arquivo << "[";

            for (int x = 0; x < imagem.largura; x++)
            {
                int posicao = (y * imagem.largura + x) * 3;

                int valor;

                if (canal == 'R')
                    valor = imagem.pixels[posicao + 2];
                else if (canal == 'G')
                    valor = imagem.pixels[posicao + 1];
                else
                    valor = imagem.pixels[posicao];

                arquivo << valor;

                if (x < imagem.largura - 1)
                    arquivo << ", ";
            }

            arquivo << "]";

            if (y < imagem.altura - 1)
                arquivo << ",";

            arquivo << endl;
        }

        arquivo << "]" << endl;
    }

    arquivo.close();

    cerr << "Arquivo " << nomeArquivo
         << " gravado com sucesso." << endl;
}

int main(int argc, char *argv[]){
        setlocale(LC_ALL, "portuguese");

        /*string nomeArquivoMPI;

        // Se o usuario informou o nome do arquivo
        //argc conta os argumentos, nesse caso o executavel + o arquivo sendo lido
        if (argc >= 2)
        {
            nomeArquivoMPI = argv[1];
            //argv cria vetores com os argumentos, sendo a posiçao 1 o segundo argumento


            // Se nao tiver .mpi no final, coloca automaticamente
            if (nomeArquivoMPI.find(".mpi") == string::npos)
            {
                nomeArquivoMPI += ".mpi";
            }
        }
        else
        {
            cerr << "Erro: nenhum arquivo MPI foi informado." << endl;
            return 1;
        }*/
    string nomeArquivoMPI = "comandos.mpi";

        // Le o arquivo MPI
        string json = LerArquivoJson(nomeArquivoMPI);



    //separar os valores de cmd´s do arquivo json
    string comando1 = PegarValorJson(json, "cmd1");
    string comando2 = PegarValorJson(json, "cmd2");
    string comando3 = PegarValorJson(json, "cmd3");
    string comando4 = PegarValorJson(json, "cmd4");
    string comando5 = PegarValorJson(json, "cmd5");
    string comando6 = PegarValorJson(json, "cmd6");

    //gravacao de json
    string nomeArquivoJson = PegarValorJson(json, "arq6");
    string canalJson = PegarValorJson(json, "canal");

    ImagemInterna imagemOriginal;
    ImagemInterna imagemRecortada;
    ImagemInterna imagemCinza;

    int x, y, largura, altura;

    string iniStr = PegarValorJson(json, "ini");
    string tamStr = PegarValorJson(json, "tam");

    ParseVetor(iniStr, x, y);
    ParseVetor(tamStr, largura, altura);

    //Abrindo a imagem
    if (comando1 == "Abra"){

        cerr << "Iniciando comando Abra..." << endl;

        imagemOriginal = AbrirImagem(json, imagemOriginal);

        cerr << "Comando Abra concluido com sucesso." << endl;
    }

    if (comando2 == "Recorta"){

        cerr << "Iniciando comando Recorta..." << endl;

        imagemRecortada = Corta(
         imagemOriginal,
            x,
            y,
            largura,
            altura
        );

        cerr << "Comando Recorta concluido com sucesso." << endl;
    }

    if (comando3 == "GravaBMP"){
        cerr << "Iniciando comando GravaBMP..." << endl;

        GravaBMP(imagemRecortada, "recorte.bmp");

        cerr << "Comando GravaBMP concluido com sucesso." << endl;
    }

    // Comando ConvGray
    if (comando4 == "ConvGray"){
        cerr << "Iniciando comando ConvGray..." << endl;

        imagemCinza = imagemRecortada;
        imagemCinza = ConvGrey(imagemCinza);

        cerr << "Comando ConvGray concluido com sucesso." << endl;
    }

    // Comando GravaBMP novamente
    if (comando5 == "GravaBMP"){
        cerr << "Iniciando comando GravaBMP..." << endl;

        GravaBMP(imagemCinza, "recorte_cinza.bmp");

        cerr << "Comando GravaBMP concluido com sucesso." << endl;
    }
    if (comando6 == "GravaJson")
    {
        cerr << "Iniciando comando GravaJson..." << endl;

        char canal = canalJson[0];

        GravaJson(
            imagemCinza,
            nomeArquivoJson,
            canal
        );

        cerr << "Comando GravaJson concluido com sucesso." << endl;
    }

    return 0;
}