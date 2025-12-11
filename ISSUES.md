# Reporting Issues

Thanks for taking the time to report an issue in **GDBS**!  
Clear and helpful bug reports allow the project to improve rapidly.

Please read this short guide before opening an issue.

---

## 🐛 Bug Reports

When something breaks, crashes, or behaves unexpectedly, please include:

### 1. What you were trying to do  
Explain the command or action that triggered the issue.

### 2. Your GDBS version  
Run:
```bash
gdbs --version
````

### 3. Your environment

For example:

* Operating system (Arch, Ubuntu, MacOS, etc.)
* Compiler used (g++, clang++, etc.)
* Installed via build.sh or self-built?

### 4. Steps to reproduce

Provide the simplest possible configuration that causes the issue.
Even a small `build.gdbs` snippet helps a lot.

### 5. Actual vs expected behavior

Tell us what happened vs what you expected.

### 6. Any error messages or output logs

Paste them as code blocks.

---

## 💡 Feature Requests

If you want to propose a feature, include:

* What problem the feature solves
* Why it would be useful
* A rough idea of how it might work
* Any examples (optional)

All suggestions are welcome — even big ideas.

---

## 📘 Documentation Issues

If you find something unclear, missing, or outdated in the docs, feel free to:

* Suggest improvements
* Open a PR
* Or describe the problem in an issue

---

## 🔥 Performance Issues

If something builds slower than you think it should:

* Describe your hardware
* Command used (e.g. `gdbs . -t 8`)
* Approximate file count
* Any build rules used (especially `combine`, `compare`, etc.)

Performance is a core goal of GDBS, so these reports are especially valuable.

---

## 🧪 Compatibility Issues

If GDBS misbehaves with:

* uncommon compilers
* filesystem quirks
* large dependency graphs
* external tools
* nested build configurations

Please describe the scenario and include any example files.

---

## 🤝 Final Notes

* Issues of all kinds are welcome
* No experience level is too small
* Clear communication helps solve problems faster

Thank you for helping make GDBS more stable, reliable, and powerful!
