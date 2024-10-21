#include "blockchain.h"
#include <chrono>
#include <iomanip>  // para put_time
#include <sstream>
#include <ctime>    // para localtime
#include "merkletree.h"
#include "listapublicaciones.h"
#include "sha256.h"
#include "arbolabb.h"

extern ArbolABB arbolabb;
extern ListaPublicaciones lista;

Blockchain::Blockchain(){
    first = nullptr;
    last = nullptr;
    size = 0;
}

int Blockchain::getSize(){
    return this->size;
}
string Blockchain::getPrehash(){
    if(this->size == 0){
        return "0000";
    }else{
        return this->last->hash;
    }
}

string getCurrentDate() {
    // Obtener el tiempo actual como time_t
    auto ahora = chrono::system_clock::now();
    time_t tiempo = chrono::system_clock::to_time_t(ahora);

    // Convertir a formato de tiempo local
    tm tiempoLocal = *localtime(&tiempo);

    // Convertir a string usando stringstream
    ostringstream streamFecha;
    streamFecha << put_time(&tiempoLocal, "%Y-%m-%d %H:%M:%S");
    return streamFecha.str();
}

void Blockchain::insert(const std::string& dataJson) {
    qDebug() << "Iniciando inserción del bloque.";

    // Crear el árbol de Merkle a partir de las publicaciones recibidas
    MerkleTree merkleTree;
    merkleTree.insert(dataJson);
    qDebug() << "Publicación insertada en MerkleTree.";

    // Obtener el hash de la raíz del árbol de Merkle
    std::string rootMerkle = merkleTree.getRootHash();
    qDebug() << "Root hash del MerkleTree: " << QString::fromStdString(rootMerkle);

    // Crear un nuevo bloque
    Block* newBlock = new Block(0);
    newBlock->index = getSize();
    newBlock->previousHash = getPrehash();
    newBlock->time = getCurrentDate();
    newBlock->rootMerkle = rootMerkle;

    // Asegurarse de que dataJson esté en el formato JSON correcto
    newBlock->data = dataJson;

    // Prueba de trabajo: encontrar un hash válido
    newBlock->mineBlock();
    qDebug() << "Hash encontrado: " << QString::fromStdString(newBlock->hash);

    // Insertar el bloque en la blockchain
    if (this->first == nullptr) {
        this->first = newBlock;
        this->last = newBlock;
    } else {
        this->last->next = newBlock;
        newBlock->back = this->last;
        this->last = newBlock;
    }
    this->size++;

    // Guardar el bloque en un archivo para persistencia
    guardarBloqueEnArchivo(newBlock);
    qDebug() << "Bloque guardado en archivo.";
}


void Blockchain::print(){


    if (this->first == nullptr) {
        qDebug() << "No hay bloques en la blockchain.";
    } else {
        Block* current = this->first;
        while(current != nullptr){
            cout << "---------------------------------------"<< endl;
            cout << "INDEX: " << current->index << endl;
            cout << "DATA: " << current->data << endl;
            cout << "TIME: " << current->time << endl;
            cout << "NONCE: " << current->nonce << endl;
            cout << "HASH: " << current->hash << endl;
            cout << "PREHASH: " << current->previousHash << endl;

            current = current->next;
        }
        qDebug() << "Imprimiendo la blockchain...";
    }

}



bool Blockchain::verificarIntegridad() {
    Block* actual = this->first;
    while (actual != nullptr) {
        string hashCalculado = actual->generateHash();
        if (hashCalculado != actual->hash) {
            // Marcar bloque como manipulado
            actual->manipulado = true;
            return false;  // La cadena está comprometida
        }
        actual = actual->next;
    }
    return true;  // La cadena está intacta
}

void Blockchain::guardarBloqueEnArchivo(Block* bloque) {
    std::string nombreArchivo = "blockchain/block_" + std::to_string(bloque->index) + ".json";
    std::ofstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo para escribir: " << nombreArchivo << std::endl;
        return;
    }

    // Convertir el bloque a JSON
    json j;
    j["index"] = bloque->index;
    j["time"] = bloque->time;
    j["data"] = json::parse(bloque->data);  // data debería ser un JSON válido
    j["nonce"] = bloque->nonce;
    j["previousHash"] = bloque->previousHash;
    j["hash"] = bloque->hash;
    j["rootMerkle"] = bloque->rootMerkle;

    archivo << j.dump(4);  // identacion
    archivo.close();
    std::cout << "Bloque guardado exitosamente en archivo: " << nombreArchivo << std::endl;
}

Block* Blockchain::getLastBlock() {
    return this->last;
}


void Blockchain::cargarBlockchain() {
    using namespace std::filesystem;

    // Verificar si la carpeta blockchain existe
    if (!exists("blockchain")) {
        std::cout << "No se encontró la carpeta 'blockchain'. Se inicializa un Blockchain vacío." << std::endl;
        return;
    }

    // Variable para almacenar el último bloque leído
    Block* ultimoBloque = nullptr;

    // Iterar sobre todos los archivos en la carpeta blockchain
    for (int i = 0; ; i++) {
        std::string nombreArchivo = "blockchain/block_" + std::to_string(i) + ".json";
        std::ifstream archivo(nombreArchivo);
        if (!archivo.is_open()) {
            break;  // Si el archivo no existe, salimos del bucle
        }

        json j;
        archivo >> j;
        archivo.close();

        // Crear un nuevo bloque basado en el archivo JSON
        Block* newBlock = new Block(0);
        newBlock->index = j["index"];
        newBlock->time = j["time"];
        newBlock->data = j["data"].dump();  // Convertir `data` a JSON serializado
        newBlock->nonce = j["nonce"];
        newBlock->previousHash = j["previousHash"];
        newBlock->hash = j["hash"];
        newBlock->rootMerkle = j["rootMerkle"];

        // Actualizar el último bloque
        ultimoBloque = newBlock;

        // Insertar el bloque en la blockchain (pero solo usaremos el último más tarde)
        if (this->first == nullptr) {
            this->first = newBlock;
            this->last = newBlock;
        } else {
            this->last->next = newBlock;
            newBlock->back = this->last;
            this->last = newBlock;
        }
        this->size++;
    }

    // Restaurar las publicaciones solo desde el último bloque cargado
    if (ultimoBloque != nullptr) {
        std::cout << "Restaurando publicaciones desde el último bloque creado." << std::endl;
        restaurarPublicacionesDesdeBloque(ultimoBloque);
    } else {
        std::cout << "No se encontró ningún bloque para restaurar." << std::endl;
    }
}

void Blockchain::restaurarPublicacionesDesdeBloque(Block* bloque) {
    try {
        std::cout << "Contenido del campo data antes de parsear: " << bloque->data << std::endl;

        // Parsear el campo `data` del bloque como JSON
        json publicacionesJson = json::parse(bloque->data);

        for (const auto& publicacionData : publicacionesJson) {
            QString correo = QString::fromStdString(publicacionData["correo"]);
            QString contenido = QString::fromStdString(publicacionData["contenido"]);
            QString fechaStr = QString::fromStdString(publicacionData["fecha"]);
            QString horaStr = QString::fromStdString(publicacionData["hora"]);
            QString rutaImagen = publicacionData.contains("imagen") ? QString::fromStdString(publicacionData["imagen"]) : QString();

            // Probar el formato de fecha y hora con segundos "HH:mm:ss"
            QDateTime fechaHora = QDateTime::fromString(fechaStr + " " + horaStr, "dd/MM/yyyy HH:mm:ss");

            // Si el formato con segundos no es válido, intentar con "HH:mm"
            if (!fechaHora.isValid()) {
                fechaHora = QDateTime::fromString(fechaStr + " " + horaStr, "dd/MM/yyyy HH:mm");
            }

            if (!fechaHora.isValid()) {
                std::cout << "Error: Fecha y hora inválidas en la publicación." << std::endl;
                continue;
            }

            // Insertar la publicación en ABB y lista doblemente enlazada
            arbolabb.insertarPublicacion(correo, contenido, rutaImagen, fechaHora.date());
            lista.agregarPublicacion(correo, contenido, rutaImagen, fechaHora);

            // Procesar los comentarios si existen
            if (publicacionData.contains("comentarios")) {
                for (const auto& comentarioData : publicacionData["comentarios"]) {
                    if (comentarioData.contains("correo") &&
                        comentarioData.contains("comentario") &&
                        comentarioData.contains("fecha") &&
                        comentarioData.contains("hora")) {

                        QString correoComentario = QString::fromStdString(comentarioData["correo"]);
                        QString textoComentario = QString::fromStdString(comentarioData["comentario"]);
                        QString fechaComentarioStr = QString::fromStdString(comentarioData["fecha"]);
                        QString horaComentarioStr = QString::fromStdString(comentarioData["hora"]);

                        // Probar formato con segundos para los comentarios
                        QDateTime fechaHoraComentario = QDateTime::fromString(fechaComentarioStr + " " + horaComentarioStr, "dd/MM/yyyy HH:mm:ss");

                        // Si el formato con segundos no es válido, intentar con "HH:mm"
                        if (!fechaHoraComentario.isValid()) {
                            fechaHoraComentario = QDateTime::fromString(fechaComentarioStr + " " + horaComentarioStr, "dd/MM/yyyy HH:mm");
                        }

                        NodoABB* nodoPublicacion = arbolabb.buscar(fechaHora.date());
                        if (nodoPublicacion) {
                            Publicacion* actualPublicacion = nodoPublicacion->listaPublicaciones;
                            while (actualPublicacion && actualPublicacion->contenido != contenido) {
                                actualPublicacion = actualPublicacion->siguiente;
                            }
                            if (actualPublicacion) {
                                actualPublicacion->arbolComentarios.insertarComentario(correoComentario, textoComentario, fechaHoraComentario);
                            }
                        }
                    }
                }
            }
        }
    } catch (const json::exception& e) {
        std::cerr << "Error al restaurar publicaciones desde el bloque: " << e.what() << std::endl;
    }
}

void Blockchain::limpiarBlockchain() {
    Block* currentBlock = first;
    while (currentBlock != nullptr) {
        Block* temp = currentBlock;
        currentBlock = currentBlock->next;
        delete temp;  // Liberar la memoria del bloque
    }
    first = nullptr;  // Restablecer la cabeza de la lista de bloques
    last = nullptr;   // Restablecer el último bloque
    size = 0;         // Reiniciar el tamaño de la blockchain
}
