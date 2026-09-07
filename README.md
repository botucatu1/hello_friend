# Hello friend
# PLEASE READ THE FULL README

It would be a 100% educational piece of malware—specifically a ransomware—designed entirely for testing in a virtual machine.


## What is it used for?

The default password is "batata"; I chose a deliberately weak password for anyone wishing to download and use the malware for educational purposes. If you want to change the password, you can download the source code and delete the existing .exe file (which is hardcoded with "batata"). To change the password, you would need an IDE—**Visual Studio Code** is a good option—and a C++ compiler, such as **g++**.
## How to download the g++ compiler?
 # Practical Guide: How to Install the g++ (GCC) Compiler

This is a quick and simple guide to help you download, extract, and install the **g++ (MinGW-w64)** compiler on Windows so you can run your C++ code.

---

## 🚀 Step 1: Download
g++ is part of the MinGW package. The most recommended official site for downloading the latest version is the *WinLibs* project GitHub page.

1. Visit the download site: [WinLibs - GCC for Windows](https://winlibs.com)
2. Look for the latest version of **GCC** (usually at the top of the page).
3. In the download table, choose the link labeled **"Zip archive"** that matches your system architecture (usually **Win64** for 64-bit systems).
4. Click the link to start downloading the `.zip` file.

## 📦 Step 2: Extraction
Once the download is complete, you will have a compressed file.

1. Go to your downloads folder and locate the downloaded `.zip` file.
2. Right-click on it and select **"Extract All..."** (or use programs like WinRAR or 7-Zip).
3. Choose a safe, easy-to-remember folder to extract the files to. It is recommended to extract them directly to your local drive, for example: `C:\mingw64`

## ⚙️ Step 3: Configuring Environment Variables
To ensure your computer recognizes the `g++` command from anywhere, you need to add its path to the system.

1. Open the folder you extracted and navigate to the folder named **`bin`** (e.g., `C:\mingw64\bin`).
2. Click the Windows Explorer address bar at the top and **copy the full path**.
3. In the Windows Start menu, search for **"Edit the system environment variables"** and open that option.
4. Click the **Environment Variables...** button at the bottom. 5. In the *System variables* section, look for the variable named **`Path`**, select it, and click **Edit...**.
6. Click **New** and paste the path you copied (e.g., `C:\mingw64\bin`).
7. Click **OK** on all open windows to save.

## ✅ Step 4: Test the Installation
To ensure everything went well, open your computer's terminal.

1. Press the `Win + R` keys, type `cmd`, and press **Enter**.
2. At the command prompt, type the following command and press Enter:
   ```bash
   g++ --version
   ```
3. If the installation was successful, the terminal will display the installed GCC version on the screen.

---
You're all set! You can now compile your `.cpp` files using the command `g++ filename.cpp -o program_name`.
##  how to use

To use this, you must run it as **administrator**; otherwise, the code will not execute correctly and will fail. So, please run it as administrator. If you aren't sure how, it's simple: right-click the .exe file, and you should see an option that says "Run as administrator"—if you see it, click it.
## IMPORTANT NOTES
I am not responsible for ANY ERRORS OR MACHINE BREAKDOWNS, NONE AT ALL! Because there are several warnings that it is educational and that it is to be used on virtual machines, and if you don't want to lose your main computer, use a virtual machine. The password is "batata" and I repeat, I AM NOT RESPONSIBLE FOR ANY ERRORS OR MACHINE BREAKDOWNS, SYSTEM ERRORS, AND ENCRYPTION PROBLEMS.
