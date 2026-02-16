# How to Push Your Project to GitHub

Your project in `release_package` is ready to be pushed. I have configured the remote for you.

## Prerequisite: API Key (Personal Access Token)
You mentioned "Here is a key", but I couldn't find it in your message. You will need your GitHub Personal Access Token (PAT) to authenticate.

## Steps to Push

1.  **Open Terminal**
    *   Open a terminal/command prompt in `c:\Users\----\Desktop\cs2-skin-changer\release_package`.

2.  **Push Code**
    *   Run the following command:
        ```powershell
        git branch -M main
        git push -u origin main
        ```
    *   When prompted for a **Username**, enter your GitHub username (e.g., `ValorantTracker` or your personal user).
    *   When prompted for a **Password**, paste your **API Key / Personal Access Token**.

## Notes
- **Remote Configured**: I have already run `git remote add origin https://github.com/ValorantTracker/cs2-skin-changer.git`.
- **License Added**: I added an MIT License file.
- **Clean**: Binaries and junk files are excluded via `.gitignore`.
