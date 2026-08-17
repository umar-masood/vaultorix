# Vaultorix

Vaultorix is a secure desktop application for storing, encrypting, and managing sensitive files locally on a user’s device.
It is built using C++ and Qt and focuses on providing strong file security through AES-256 encryption, OTP-based authentication, and secure password storage. The application is designed to keep all user data local and under user control.

## Features
### Authentication

* User registration and login
* OTP-based two-factor authentication
* Password hashing with salt
* Protection against repeated invalid OTP attempts
* Account lockout after multiple failed attempts

### File Encryption

* AES-256 encryption and decryption
* Secure local file processing
* Encryption status tracking
* File metadata management

### File Management

* Add files to vault
* Encrypt and decrypt files
* Delete files from vault
* Open file location
* View stored file details

### User Interface

* Desktop-based interface using Qt
* Dashboard for file management
* Light and dark themes
* Profile and settings management

### Updates

* Update checking system
* Planned maintenance and improvements

## Security and Privacy

Vaultorix is designed with a local-first security model.

* All files remain on the user’s device
* Encryption keys are not stored on external servers
* Encrypted files are not uploaded to any cloud service
* Passwords are stored using secure hashing with salt
* OTP verification is handled through external email services

Users are responsible for maintaining backups of their data.

## Screenshots
<img width="1365" height="767" alt="A" src="https://github.com/user-attachments/assets/d7eb046b-7fff-4cd0-a4f8-bbd7886d90d1" />
<img width="1365" height="765" alt="B" src="https://github.com/user-attachments/assets/a2e40800-0380-48b3-9c70-21bde05896a3" />
<img width="1365" height="757" alt="C" src="https://github.com/user-attachments/assets/dbce8569-a929-4445-bde0-69c8dfb267ba" />
<img width="1365" height="745" alt="C (1)" src="https://github.com/user-attachments/assets/fe02e30a-72fc-4021-b8d5-f2bc9e014086" />
<img width="1365" height="746" alt="D" src="https://github.com/user-attachments/assets/57f6ce9e-7109-4729-af66-360ac3a9a376" />
<img width="1365" height="746" alt="E" src="https://github.com/user-attachments/assets/5d22765e-ff10-4201-99ab-fe60019b9005" />
<img width="1365" height="745" alt="F" src="https://github.com/user-attachments/assets/0df13deb-aff3-4dbe-85bc-8587bd69ffcf" />
<img width="1365" height="732" alt="G" src="https://github.com/user-attachments/assets/81cc6ec1-1ff2-4ac1-bab8-4c075c77eb6d" />


## Architecture
Vaultorix is built using a modular structure:
* Authentication module (login, signup, OTP, account security)
* Encryption module (AES-256 encryption and decryption)
* Storage module (file handling and metadata)
* User interface module (dashboard, settings, themes)
* Services module (authentication services, updates, future features)

This structure allows each component to be developed and maintained independently.

## Technology Stack

* C++ (core application logic)
* Qt (desktop application framework)
* QSS (UI styling)
* PostgreSQL (database)
* FastAPI (backend services)
* Python (supporting services)
* Supabase (authentication and backend services)
* Brevo (email and OTP delivery)
* Git / GitHub (version control)

## Platform Support
Window, MacOS and Linux

## Installation
Installation instructions will be provided in the first public release.

For development:

```bash
git clone https://github.com/umar-masood/Vaultorix.git
```

## Development
Vaultorix follows an iterative development approach.
Guidelines for contributors:

* Follow existing project structure
* Keep modules separated by responsibility
* Avoid unnecessary dependencies
* Maintain security practices
* Include testing for changes where needed

## Project Status
Vaultorix is currently in active development.
The core system is functional, while additional features such as backup, licensing, and cross-platform support are planned for future releases.

