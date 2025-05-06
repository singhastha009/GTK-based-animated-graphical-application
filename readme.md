# 🌸 GTK Animation Project in C

An animated graphical desktop application built with C, GTK, and Cairo. This project features floating particles, a falling circle animation, on-screen text, and a rotating/scaling image reveal — all drawn and animated manually using Cairo within a GTK window.

---

## 🎯 Features

- Gradient background and floating "flower" particles  
- A circle that drops into view followed by a motivational message  
- Smooth transition into an animated image that scales and rotates  
- Built using low-level drawing and animation logic (no game engine)

---

## 🛠 Technologies Used

- **Language:** C  
- **GUI Framework:** GTK 4  
- **Graphics Library:** Cairo  
- **Image Handling:** GDK Pixbuf  
- **Build Tool:** `gcc` / `make`

---

## 🚀 How to Build and Run

### 1. Dependencies

Ensure GTK 4 is installed:

**On Ubuntu/Debian:**
```bash
sudo apt install libgtk-4-dev

##On MacOS
bash - brew install gtk4

Compile - gcc animation.c -o animation `pkg-config --cflags --libs gtk4` -lm
Run - ./animation



