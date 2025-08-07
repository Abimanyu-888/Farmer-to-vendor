# C++ Farm-to-Vendor Platform

This project is a C++-based backend for a farm-to-vendor marketplace, developed as a final project for a data structures course. The platform exposes a RESTful API to manage users (farmers and vendors), products, and orders.

The primary academic goal is the implementation of core data structures from scratch to handle the application's data management, rather than relying on standard library containers.

## 📜 Project Description

The application functions as a monolithic C++ backend server. It provides a complete set of API endpoints for creating and managing users, listing products for sale, and placing orders. To align with the project's focus on data structures, all platform data is held in-memory using custom-built data structures. Data persistence is achieved by serializing these structures to JSON files on server shutdown and deserializing them on startup.

## 🛠️ Technology Stack

The project is built with a focus on simplicity, cross-platform compatibility, and a low barrier to entry for developers.

* **Language:** C++ (Modern)
* **Web Framework:** [Crow](https://crowcpp.org/) (A fast, lightweight, header-only microframework)
* **Unit Testing:** [Catch2](https://github.com/catchorg/Catch2) (A modern, C++-native, test framework)
* **Documentation:** LaTeX (For creating the final project report and formal documentation)
* **Compiler:** [g++](https://gcc.gnu.org/) (The GNU Compiler Collection)
* **Version Control:** Git & GitHub
* **Recommended IDE:** Visual Studio Code with the C/C++ extension pack.

## 🏗️ System Architecture

The architecture is designed to be simple and robust, avoiding the complexities of external databases or microservices.

1.  **Monolithic Backend:** A single C++ executable that runs the web server and contains all business logic.
2.  **RESTful API:** Exposes HTTP endpoints for all Create, Read, Update, and Delete (CRUD) operations.
3.  **In-Memory Data Management:** Core platform entities (Users, Products, Orders) are stored in custom data structures in memory for fast access.
    * **Users & Products:** Managed by a custom **Hash Table** implementation for O(1) average time complexity on lookups, insertions, and deletions.
    * **Orders:** Managed by a custom **Singly Linked List** attached to each user object to store their order history.
4.  **JSON Persistence:** The state of the in-memory data structures is saved to and loaded from JSON files on the disk, providing a simple persistence layer.

## 🔌 API Endpoints

The following RESTful endpoints are provided:

| Method | Endpoint                       | Description                                      |
| :----- | :----------------------------- | :----------------------------------------------- |
| `POST` | `/users`                       | Create a new user (farmer or vendor).            |
| `GET`  | `/users`                       | Get a list of all users.                         |
| `GET`  | `/users/{userId}`              | Get details for a specific user.                 |
| `POST` | `/products`                    | Create a new product for a farmer.               |
| `GET`  | `/products`                    | Get all products. Supports filtering by name.    |
| `GET`  | `/products?name={query}`       | Search for products by name.                     |
| `POST` | `/orders`                      | Place a new order for a product.                 |
| `GET`  | `/users/{userId}/orders`       | Get the order history for a specific user.       |
| `PUT`  | `/orders/{orderId}`            | Update the status of an order (e.g., FULFILLED). |

## 🚀 Getting Started

Follow these instructions to get a local copy up and running for development and testing.

### Prerequisites

Ensure you have the following tools installed on your system:
* A modern C++ compiler (g++)
* Git
* CMake (for building the project)

### Build and Run

1.  **Clone the repository:**
    ```sh
    git clone [https://github.com/your-username/your-repo-name.git](https://github.com/your-username/your-repo-name.git)
    cd your-repo-name
    ```

2.  **Configure and build the project using CMake:**
    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```

3.  **Run the server:**
    The executable will be located in the `build` directory.
    ```sh
    ./farm-to-vendor-app
    ```
    The server will start and be accessible at `http://localhost:18080`.

4.  **Run tests:**
    To run the unit tests built with Catch2:
    ```sh
    ./run_tests
    ```

## 📈 Project Status

This project is currently in development.
