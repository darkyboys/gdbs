# Contributing to GDBS

Thanks for your interest in contributing to **Goblin's Demonic Build System (GDBS)**!  
This project is open to everyone — whether you're fixing a typo, optimizing the core, or proposing new features.

GDBS is still evolving, so contributions of all sizes are welcome.

---

## 📌 Ways You Can Contribute

### ✔ Report bugs
If something breaks, crashes, behaves weirdly, or feels unexpected, please file an issue.  
Your reports directly help improve the stability of the build system.

### ✔ Suggest new features
Have an idea that could improve GDBS?  
Open an issue and describe:

- What problem the feature solves  
- Why it would be useful  
- How you imagine it working  

Even rough ideas are welcome.

### ✔ Improve documentation
If you spot unclear sections, typos, formatting issues, or missing examples, feel free to update the docs.  
Good documentation makes GDBS easier for everyone to use.

### ✔ Contribute code
You can submit pull requests for:

- Bug fixes  
- Performance improvements  
- New features  
- Refactoring / code cleanup  
- Better error messages  
- Config format enhancements (H699 improvements)

---

## 📦 Development Setup

1. Clone the repository:
```bash
git clone https://github.com/darkyboys/gdbs
cd gdbs
```

2. Build GDBS:

```bash
chmod +x build.sh
./build.sh
```

3. Run GDBS locally:

```bash
./gdbs .
```

---

## 🧭 Coding Guidelines (Loose but Helpful)

GDBS is written entirely in **C++20 with standard STL only**.
To keep the project consistent:

* Use clear, readable code.
* Prefer standard library utilities over custom structures.
* Keep performance in mind — GDBS’s speed matters.
* Avoid unnecessary abstractions.
* Document complex code paths when needed.
* Keep cross-platform behavior in mind.

No strict formatting rules.
Just write clean and understandable C++.

 > Note: To be safe from the Supply Chain Attacks, It is compulsory for every code contribution to come with a proper comment from where you started , and where you ended and what you did. without those the code based modifications would be rejected (In some cases if the code changes are simple then they might still be accepted). This only applies to the code based changes not other contributions, Also you must not add any kind of custom executable binary inside the pull requests or those will be rejected , Either keep the executale a shell file (Properly commented or very simple and small one)

---

## 🔄 Pull Request Process

1. Fork the repository
2. Create a branch for your fix or feature
3. Commit clearly and concisely
4. Submit a PR and describe your change
5. A maintainer will review and merge, or request adjustments

There is **no strict review pressure** — contributors of all experience levels are welcome.

---

## 💬 Communication

If you're unsure about something:

* Open an issue
* Drop a question
* Or propose the idea directly in a PR

We aim to keep the project friendly, open, and easy to contribute to.

---

## 🖤 Thank You

Every contribution — big or small — helps make GDBS faster, better, and more powerful.
Thank you for taking the time to improve this project!