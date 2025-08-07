document.addEventListener('DOMContentLoaded', () => {
    // The base URL for your C++ backend API.
    // Ensure this port matches the one your Crow app is running on.
    const API_BASE_URL = 'http://localhost:18080';

    const loadProductsBtn = document.getElementById('load-products-btn');
    const productsContainer = document.getElementById('products-container');
    const registerForm = document.getElementById('register-form');
    const orderForm = document.getElementById('order-form');

    /**
     * Fetches all products from the /products endpoint and displays them.
     */
    const fetchProducts = async () => {
        productsContainer.innerHTML = '<p>Loading products...</p>';
        try {
            const response = await fetch(`${API_BASE_URL}/products`);
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            // Assuming the backend returns an array of products.
            // If the backend returns a JSON object with a 'products' key, adjust accordingly.
            const products = await response.json();

            productsContainer.innerHTML = ''; // Clear loading message
            if (!products || products.length === 0) {
                productsContainer.innerHTML = '<p>No products are currently available.</p>';
                return;
            }

            products.forEach(product => {
                const productCard = document.createElement('div');
                productCard.className = 'product-card';
                productCard.innerHTML = `
                    <h3>${product.name || 'Unnamed Product'}</h3>
                    <p><strong>ID:</strong> ${product.productId}</p>
                    <p><strong>Price:</strong> $${(product.price || 0).toFixed(2)}</p>
                    <p><strong>Farmer ID:</strong> ${product.farmerId}</p>
                    <p>${product.description || 'No description available.'}</p>
                `;
                productsContainer.appendChild(productCard);
            });
        } catch (error) {
            productsContainer.innerHTML = `<p style="color: red;">Error loading products: ${error.message}</p>`;
            console.error('Fetch error:', error);
        }
    };

    /**
     * Handles the submission of the user registration form.
     * Sends a POST request to the /users endpoint.
     */
    const handleRegistration = async (event) => {
        event.preventDefault();
        const formData = new FormData(registerForm);
        const userData = Object.fromEntries(formData.entries());

        try {
            const response = await fetch(`${API_BASE_URL}/users`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(userData),
            }); // [3, 4]

            const result = await response.json();
            if (!response.ok) {
                // Use the error message from the backend if available
                throw new Error(result.error || `HTTP error! status: ${response.status}`);
            }
            alert(`User registered successfully! Your User ID is: ${result.userId}`);
            registerForm.reset();
        } catch (error) {
            alert(`Registration failed: ${error.message}`);
            console.error('Registration error:', error);
        }
    };

    /**
     * Handles the submission of the order placement form.
     * Sends a POST request to the /orders endpoint.
     */
    const handleOrderPlacement = async (event) => {
        event.preventDefault();
        const formData = new FormData(orderForm);
        const orderData = Object.fromEntries(formData.entries());
        // Ensure quantity is sent as a number, not a string
        orderData.quantity = parseInt(orderData.quantity, 10);

        try {
            const response = await fetch(`${API_BASE_URL}/orders`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(orderData),
            }); // [5]

            const result = await response.json();
            if (!response.ok) {
                throw new Error(result.error || `HTTP error! status: ${response.status}`);
            }
            alert(`Order placed successfully! Your Order ID is: ${result.orderId}`);
            orderForm.reset();
        } catch (error) {
            alert(`Order placement failed: ${error.message}`);
            console.error('Order error:', error);
        }
    };

    // Add event listeners to the buttons and forms
    loadProductsBtn.addEventListener('click', fetchProducts);
    registerForm.addEventListener('submit', handleRegistration);
    orderForm.addEventListener('submit', handleOrderPlacement);
});