#include <iostream>
#include <limits>
#include <vector>
#include <iomanip>
#include <string>
#include <fstream>
#include <numeric>
#include <sstream>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <set>
#include <functional>
using namespace std;

// Define a struct for recycled products
struct RecycledProduct {
    string wasteType;
    string productName;
    double price;
};

// Define a struct for the user cart
struct CartItem {
    RecycledProduct product;
    int quantity;
};

// Define a graph for waste-product relationships
class ProductGraph {
private:
    unordered_map<string, vector<string>> adjList;

public:
    void addEdge(const string& wasteType, const string& productName) {
        adjList[wasteType].push_back(productName);
    }

    void displayGraph() const {
        cout << "\nWaste-Product Relationships:" << endl;
        for (const auto& pair : adjList) {
            cout << pair.first << " -> ";
            for (const auto& product : pair.second) {
                cout << product << ", ";
            }
            cout << endl;
        }
    }

    vector<string> getRelatedProducts(const string& wasteType) const {
        auto it = adjList.find(wasteType);
        if (it != adjList.end()) {
            return it->second;
        }
        return {};
    }

    void bfsTraversal(const string& start) const {
        if (adjList.find(start) == adjList.end()) {
            cout << "\nNo products found for this waste type." << endl;
            return;
        }

        cout << "\nBFS Traversal from " << start << ":\n";
        queue<string> q;
        set<string> visited;

        q.push(start);
        visited.insert(start);

        while (!q.empty()) {
            string current = q.front();
            q.pop();

            cout << current << " -> ";

            for (const auto& neighbor : adjList.at(current)) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    q.push(neighbor);
                }
            }
        }
        cout << "END" << endl;
    }

    void dfsTraversal(const string& start) const {
        unordered_map<string, bool> visited;

        function<void(const string&)> dfs = [&](const string& node) {
            visited[node] = true;
            cout << node << " -> ";
            for (const auto& neighbor : getRelatedProducts(node)) {
                if (!visited[neighbor]) {
                    dfs(neighbor);
                }
            }
        };

        if (getRelatedProducts(start).empty()) {
            cout << "\nNo related products found for the specified waste type." << endl;
        } else {
            cout << "\nDFS Traversal from " << start << ":\n";
            dfs(start);
            cout << "END" << endl;
        }
    }
};

// Function prototypes

void loadFromFile(const string& daa);
void selectCompany();
void displayProducts(const vector<RecycledProduct>& products);
void selectProduct(vector<RecycledProduct>& products, vector<CartItem>& cart);
void displayCart(const vector<CartItem>& cart);
void checkout(const vector<CartItem>& cart);
void loadProducts(vector<RecycledProduct>& products);
void saveCart(const vector<CartItem>& cart);
void loadCart(vector<CartItem>& cart, const vector<RecycledProduct>& products);
void setupGraph(ProductGraph& graph, const vector<RecycledProduct>& products);
bool isValidWasteType(const string& wasteType, const vector<RecycledProduct>& products);
void suggestWasteType(const vector<RecycledProduct>& products);
bool isConnected(const ProductGraph& graph, const string& waste1, const string& waste2);


void loadFromFile(const string& daa) {
    ifstream file(daa);
    if (!file.is_open())
        {

        cerr << "Failed to open the file: " << daa << endl;
        return;
    }

    string line;
    cout << "\nContents of the file " << daa << ":\n";
    while (getline(file, line)) {
        cout << line << endl;
    }
    file.close();
}




void displayProducts(const vector<RecycledProduct>& products) {
    if (products.empty()) {
        cout << "\nNo products available to display." << endl;
        return;
    }

    vector<RecycledProduct> filteredProducts = products;
    int filterChoice, sortChoice;
    string filterValue;

    // Step 1: Filter Options
    cout << "\nFilter Options:\n";
    cout << "1. Show All Products (No Filter)\n";
    cout << "2. Filter by Waste Type\n";
    cout << "3. Filter by Product Name Keyword\n";
    cout << "4. Filter by Price Range\n";
    cout << "Enter your filter choice: ";
    cin >> filterChoice;

    // Apply Filtering
    switch (filterChoice) {
        case 2: {
            cout << "\nEnter Waste Type to filter: ";
            cin >> filterValue;
            filteredProducts.erase(remove_if(filteredProducts.begin(), filteredProducts.end(),
                                             [&](const RecycledProduct& p) { return p.wasteType != filterValue; }),
                                   filteredProducts.end());
            break;
        }
        case 3: {
            cout << "\nEnter Product Name Keyword to filter: ";
            cin.ignore(); // Clear buffer
            getline(cin, filterValue);
            filteredProducts.erase(remove_if(filteredProducts.begin(), filteredProducts.end(),
                                             [&](const RecycledProduct& p) {
                                                 return p.productName.find(filterValue) == string::npos;
                                             }),
                                   filteredProducts.end());
            break;
        }
        case 4: {
            double minPrice, maxPrice;
            cout << "\nEnter minimum price: ";
            cin >> minPrice;
            cout << "Enter maximum price: ";
            cin >> maxPrice;
            filteredProducts.erase(remove_if(filteredProducts.begin(), filteredProducts.end(),
                                             [&](const RecycledProduct& p) {
                                                 return p.price < minPrice || p.price > maxPrice;
                                             }),
                                   filteredProducts.end());
            break;
        }
        case 1:
        default:
            break; // No filtering
    }

    if (filteredProducts.empty()) {
        cout << "\nNo products match the selected filter criteria." << endl;
        return;
    }

    // Step 2: Sort Options
    cout << "\nSort Options:\n";
    cout << "1. Sort by Waste Type (A-Z)\n";
    cout << "2. Sort by Product Name (A-Z)\n";
    cout << "3. Sort by Price (Low to High)\n";
    cout << "4. Sort by Price (High to Low)\n";
    cout << "Enter your sorting choice: ";
    cin >> sortChoice;

    // Apply Sorting
    switch (sortChoice) {
        case 1:
            sort(filteredProducts.begin(), filteredProducts.end(),
                 [](const RecycledProduct& a, const RecycledProduct& b) { return a.wasteType < b.wasteType; });
            break;
        case 2:
            sort(filteredProducts.begin(), filteredProducts.end(),
                 [](const RecycledProduct& a, const RecycledProduct& b) { return a.productName < b.productName; });
            break;
        case 3:
            sort(filteredProducts.begin(), filteredProducts.end(),
                 [](const RecycledProduct& a, const RecycledProduct& b) { return a.price < b.price; });
            break;
        case 4:
            sort(filteredProducts.begin(), filteredProducts.end(),
                 [](const RecycledProduct& a, const RecycledProduct& b) { return a.price > b.price; });
            break;
        default:
            cout << "\nInvalid sorting choice. Displaying products without sorting." << endl;
            break;
    }

    // Step 3: Display Products
    cout << "\nFiltered and Sorted Recycled Products:\n";
    cout << left << setw(5) << "No." << setw(20) << "Waste Type" << setw(25) << "Product Name" << "Price (in Rupees)" << endl;
    cout << string(60, '-') << endl;

    for (size_t i = 0; i < filteredProducts.size(); ++i) {
        cout << left << setw(5) << i + 1 << setw(20) << filteredProducts[i].wasteType
             << setw(25) << filteredProducts[i].productName << filteredProducts[i].price << endl;
    }

    // Step 4: Export Options
    char exportChoice;
    cout << "\nWould you like to export the displayed products to a file? (y/n): ";
    cin >> exportChoice;

    if (exportChoice == 'y' || exportChoice == 'Y') {
        ofstream file("filtered_products.txt");
        if (!file) {
            cerr << "\nFailed to create the file." << endl;
            return;
        }

        file << left << setw(5) << "No." << setw(20) << "Waste Type" << setw(25) << "Product Name" << "Price (in Rupees)" << endl;
        file << string(60, '-') << endl;
        for (size_t i = 0; i < filteredProducts.size(); ++i) {
            file << left << setw(5) << i + 1 << setw(20) << filteredProducts[i].wasteType
                 << setw(25) << filteredProducts[i].productName << filteredProducts[i].price << endl;
        }

        cout << "\nProducts successfully exported to 'filtered_products.txt'." << endl;
    }

    // Step 5: Additional Analysis
    cout << "\nAdditional Analysis Options:\n";
    cout << "1. Find the Most Expensive Product\n";
    cout << "2. Find the Cheapest Product\n";
    cout << "3. Calculate Average Product Price\n";
    cout << "Enter your choice (or 0 to skip): ";
    int analysisChoice;
    cin >> analysisChoice;

    switch (analysisChoice) {
        case 1: {
            auto maxProduct = max_element(filteredProducts.begin(), filteredProducts.end(),
                                          [](const RecycledProduct& a, const RecycledProduct& b) { return a.price < b.price; });
            cout << "\nMost Expensive Product:\n";
            cout << "Waste Type: " << maxProduct->wasteType << "\nProduct Name: " << maxProduct->productName
                 << "\nPrice: " << maxProduct->price << " Rupees" << endl;
            break;
        }
        case 2: {
            auto minProduct = min_element(filteredProducts.begin(), filteredProducts.end(),
                                          [](const RecycledProduct& a, const RecycledProduct& b) { return a.price < b.price; });
            cout << "\nCheapest Product:\n";
            cout << "Waste Type: " << minProduct->wasteType << "\nProduct Name: " << minProduct->productName
                 << "\nPrice: " << minProduct->price << " Rupees" << endl;
            break;
        }
        case 3: {
            double totalPrice = accumulate(filteredProducts.begin(), filteredProducts.end(), 0.0,
                                           [](double sum, const RecycledProduct& p) { return sum + p.price; });
            double averagePrice = totalPrice / filteredProducts.size();
            cout << "\nAverage Product Price: " << averagePrice << " Rupees" << endl;
            break;
        }
        case 0:
        default:
            cout << "\nSkipping additional analysis." << endl;
            break;
    }
}



// Function to handle product selection and add to cart
void selectProduct(vector<RecycledProduct>& products, vector<CartItem>& cart) {
    if (products.empty()) {
        cout << "\nNo products available for selection." << endl;
        return;
    }

    int choice;
    string searchKeyword;
    char action;
    bool productSelected = false;

    while (!productSelected) {
        cout << "\nProduct Selection Menu:" << endl;
        cout << "1. Search for a product by keyword" << endl;
        cout << "2. View all products and select" << endl;
        cout << "3. Exit selection" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                cin.ignore(); // Clear buffer
                cout << "\nEnter a keyword to search for a product: ";
                getline(cin, searchKeyword);

                vector<int> matchingIndexes;
                for (size_t i = 0; i < products.size(); ++i) {
                    if (products[i].productName.find(searchKeyword) != string::npos ||
                        products[i].wasteType.find(searchKeyword) != string::npos) {
                        matchingIndexes.push_back(i);
                    }
                }

                if (matchingIndexes.empty()) {
                    cout << "\nNo products found matching the keyword \"" << searchKeyword << "\"." << endl;
                } else {
                    cout << "\nMatching Products:" << endl;
                    cout << left << setw(5) << "No." << setw(20) << "Waste Type" << setw(25) << "Product Name" << "Price (in Rupees)" << endl;
                    cout << string(60, '-') << endl;
                    for (size_t i = 0; i < matchingIndexes.size(); ++i) {
                        const auto& product = products[matchingIndexes[i]];
                        cout << left << setw(5) << i + 1 << setw(20) << product.wasteType
                             << setw(25) << product.productName << product.price << endl;
                    }

                    cout << "\nEnter the number corresponding to the product to select it, or 0 to cancel: ";
                    int selectedIndex;
                    cin >> selectedIndex;

                    if (selectedIndex > 0 && selectedIndex <= static_cast<int>(matchingIndexes.size())) {
                        const RecycledProduct& selectedProduct = products[matchingIndexes[selectedIndex - 1]];
                        int quantity;
                        cout << "\nSelected Product Details:" << endl;
                        cout << "Waste Type: " << selectedProduct.wasteType << endl;
                        cout << "Product Name: " << selectedProduct.productName << endl;
                        cout << "Price: " << selectedProduct.price << " Rupees" << endl;
                        cout << "Enter quantity to add to cart: ";
                        cin >> quantity;

                        if (quantity > 0) {
                            cart.push_back({selectedProduct, quantity});
                            cout << "\nAdded " << quantity << " x " << selectedProduct.productName << " to your cart." << endl;
                            productSelected = true;
                        } else {
                            cout << "\nInvalid quantity. Please try again." << endl;
                        }
                    } else if (selectedIndex == 0) {
                        cout << "\nProduct selection cancelled." << endl;
                    } else {
                        cout << "\nInvalid choice. Please try again." << endl;
                    }
                }
                break;
            }

            case 2: {
                cout << "\nAvailable Products:" << endl;
                cout << left << setw(5) << "No." << setw(20) << "Waste Type" << setw(25) << "Product Name" << "Price (in Rupees)" << endl;
                cout << string(60, '-') << endl;
                for (size_t i = 0; i < products.size(); ++i) {
                    cout << left << setw(5) << i + 1 << setw(20) << products[i].wasteType
                         << setw(25) << products[i].productName << products[i].price << endl;
                }

                cout << "\nEnter the number corresponding to the product you want to select: ";
                int selectedIndex;
                cin >> selectedIndex;

                if (selectedIndex > 0 && selectedIndex <= static_cast<int>(products.size())) {
                    const RecycledProduct& selectedProduct = products[selectedIndex - 1];
                    int quantity;
                    cout << "\nSelected Product Details:" << endl;
                    cout << "Waste Type: " << selectedProduct.wasteType << endl;
                    cout << "Product Name: " << selectedProduct.productName << endl;
                    cout << "Price: " << selectedProduct.price << " Rupees" << endl;
                    cout << "Enter quantity to add to cart: ";
                    cin >> quantity;

                    if (quantity > 0) {
                        cart.push_back({selectedProduct, quantity});
                        cout << "\nAdded " << quantity << " x " << selectedProduct.productName << " to your cart." << endl;
                        productSelected = true;
                    } else {
                        cout << "\nInvalid quantity. Please try again." << endl;
                    }
                } else {
                    cout << "\nInvalid choice. Please try again." << endl;
                }
                break;
            }

            case 3:
                cout << "\nExiting product selection menu." << endl;
                return;

            default:
                cout << "\nInvalid choice. Please try again." << endl;
                break;
        }
    }
}

// Function to display the cart contents
void displayCart(const vector<CartItem>& cart) {
    if (cart.empty()) {
        cout << "\nYour cart is empty." << endl;
        return;
    }

    cout << "\nYour Cart:\n";
    cout << left << setw(5) << "No." << setw(25) << "Product Name" << setw(15) << "Waste Type"
         << setw(10) << "Quantity" << "Total Price (in Rupees)" << endl;
    cout << string(80, '-') << endl;

    double grandTotal = 0.0;
    unordered_map<string, double> categoryTotals; // To calculate total price per waste type
    unordered_map<string, int> categoryQuantities; // To calculate quantity per waste type

    for (size_t i = 0; i < cart.size(); ++i) {
        double total = cart[i].quantity * cart[i].product.price;
        grandTotal += total;
        categoryTotals[cart[i].product.wasteType] += total;
        categoryQuantities[cart[i].product.wasteType] += cart[i].quantity;

        cout << left << setw(5) << i + 1 << setw(25) << cart[i].product.productName
             << setw(15) << cart[i].product.wasteType << setw(10) << cart[i].quantity << total << endl;
    }

    cout << "\nGrand Total: " << grandTotal << " Rupees" << endl;

    // Display category-wise breakdown
    cout << "\nCategory-Wise Breakdown:\n";
    cout << left << setw(20) << "Waste Type" << setw(15) << "Total Quantity"
         << "Total Price (in Rupees)" << endl;
    cout << string(60, '-') << endl;
    for (const auto& [wasteType, totalPrice] : categoryTotals) {
        cout << left << setw(20) << wasteType << setw(15) << categoryQuantities[wasteType] << totalPrice << endl;
    }

    // Additional analysis
    cout << "\nAdditional Analysis Options:\n";
    cout << "1. Most Purchased Product\n";
    cout << "2. Most Expensive Product in Cart\n";
    cout << "3. Cheapest Product in Cart\n";
    cout << "4. Average Price of Items in Cart\n";
    cout << "5. Export Cart Details to File\n";
    cout << "Enter your choice (or 0 to skip): ";
    int analysisChoice;
    cin >> analysisChoice;

    switch (analysisChoice) {
        case 1: {
            auto maxItem = max_element(cart.begin(), cart.end(),
                                       [](const CartItem& a, const CartItem& b) {
                                           return a.quantity < b.quantity;
                                       });
            cout << "\nMost Purchased Product:\n";
            cout << "Product Name: " << maxItem->product.productName << "\nWaste Type: "
                 << maxItem->product.wasteType << "\nQuantity: " << maxItem->quantity << endl;
            break;
        }
        case 2: {
            auto maxPriceItem = max_element(cart.begin(), cart.end(),
                                            [](const CartItem& a, const CartItem& b) {
                                                return (a.quantity * a.product.price) <
                                                       (b.quantity * b.product.price);
                                            });
            cout << "\nMost Expensive Product in Cart:\n";
            cout << "Product Name: " << maxPriceItem->product.productName << "\nWaste Type: "
                 << maxPriceItem->product.wasteType << "\nTotal Price: "
                 << (maxPriceItem->quantity * maxPriceItem->product.price) << " Rupees" << endl;
            break;
        }
        case 3: {
            auto minPriceItem = min_element(cart.begin(), cart.end(),
                                            [](const CartItem& a, const CartItem& b) {
                                                return (a.quantity * a.product.price) <
                                                       (b.quantity * b.product.price);
                                            });
            cout << "\nCheapest Product in Cart:\n";
            cout << "Product Name: " << minPriceItem->product.productName << "\nWaste Type: "
                 << minPriceItem->product.wasteType << "\nTotal Price: "
                 << (minPriceItem->quantity * minPriceItem->product.price) << " Rupees" << endl;
            break;
        }
        case 4: {
            double totalItems = 0;
            for (const auto& item : cart) {
                totalItems += item.quantity;
            }
            double averagePrice = (totalItems > 0) ? grandTotal / totalItems : 0.0;
            cout << "\nAverage Price of Items in Cart: " << averagePrice << " Rupees" << endl;
            break;
        }
        case 5: {
            ofstream file("cart_details.txt");
            if (!file) {
                cerr << "\nFailed to export cart details." << endl;
                return;
            }

            file << left << setw(5) << "No." << setw(25) << "Product Name" << setw(15) << "Waste Type"
                 << setw(10) << "Quantity" << "Total Price (in Rupees)" << endl;
            file << string(80, '-') << endl;

            for (size_t i = 0; i < cart.size(); ++i) {
                double total = cart[i].quantity * cart[i].product.price;
                file << left << setw(5) << i + 1 << setw(25) << cart[i].product.productName
                     << setw(15) << cart[i].product.wasteType << setw(10) << cart[i].quantity << total << endl;
            }

            file << "\nGrand Total: " << grandTotal << " Rupees\n";

            file << "\nCategory-Wise Breakdown:\n";
            file << left << setw(20) << "Waste Type" << setw(15) << "Total Quantity"
                 << "Total Price (in Rupees)" << endl;
            file << string(60, '-') << endl;
            for (const auto& [wasteType, totalPrice] : categoryTotals) {
                file << left << setw(20) << wasteType << setw(15) << categoryQuantities[wasteType] << totalPrice
                     << endl;
            }

            cout << "\nCart details successfully exported to 'cart_details.txt'." << endl;
            break;
        }
        case 0:
        default:
            cout << "\nSkipping additional analysis." << endl;
            break;
    }
}


// Function to handle checkout process
// Function to handle the checkout process
void checkout(const vector<CartItem>& cart) {
    if (cart.empty()) {
        cout << "\nYour cart is empty. Nothing to checkout." << endl;
        return;
    }

    double grandTotal = 0.0;
    for (const auto& item : cart) {
        grandTotal += item.quantity * item.product.price;
    }

    // Apply discounts
    double discount = 0.0;
    if (grandTotal > 1000.0) {
        discount = grandTotal * 0.10; // 10% discount for purchases above 1000
        cout << "\nCongratulations! You received a 10% discount for spending over 1000.\n";
    }

    double finalTotal = grandTotal - discount;

    // Display detailed receipt
    cout << "\n--- Checkout Receipt ---\n";
    cout << left << setw(5) << "No." << setw(25) << "Product Name" << setw(10) << "Quantity"
         << setw(15) << "Unit Price" << "Total Price" << endl;
    cout << string(70, '-') << endl;

    for (size_t i = 0; i < cart.size(); ++i) {
        double total = cart[i].quantity * cart[i].product.price;
        cout << left << setw(5) << i + 1 << setw(25) << cart[i].product.productName
             << setw(10) << cart[i].quantity << setw(15) << cart[i].product.price << total << endl;
    }

    cout << "\nSubtotal: " << grandTotal << " Rupees";
    if (discount > 0.0) {
        cout << "\nDiscount: " << discount << " Rupees";
    }
    cout << "\nFinal Total: " << finalTotal << " Rupees\n";

    // Save receipt to a file
    char saveReceipt;
    cout << "\nWould you like to save the receipt to a file? (y/n): ";
    cin >> saveReceipt;

    if (saveReceipt == 'y' || saveReceipt == 'Y') {
        ofstream receiptFile("receipt.txt");
        if (!receiptFile) {
            cerr << "Failed to save the receipt." << endl;
        } else {
            receiptFile << "--- Checkout Receipt ---\n";
            receiptFile << left << setw(5) << "No." << setw(25) << "Product Name" << setw(10) << "Quantity"
                        << setw(15) << "Unit Price" << "Total Price" << endl;
            receiptFile << string(70, '-') << endl;

            for (size_t i = 0; i < cart.size(); ++i) {
                double total = cart[i].quantity * cart[i].product.price;
                receiptFile << left << setw(5) << i + 1 << setw(25) << cart[i].product.productName
                            << setw(10) << cart[i].quantity << setw(15) << cart[i].product.price << total << endl;
            }

            receiptFile << "\nSubtotal: " << grandTotal << " Rupees\n";
            if (discount > 0.0) {
                receiptFile << "Discount: " << discount << " Rupees\n";
            }
            receiptFile << "Final Total: " << finalTotal << " Rupees\n";
            cout << "Receipt saved successfully to 'receipt.txt'.\n";
        }
    }

    // Payment processing
    int paymentChoice;
    cout << "\nSelect Payment Method:\n";
    cout << "1. Credit Card\n";
    cout << "2. Debit Card\n";
    cout << "3. Cash\n";
    cout << "4. Digital Wallet\n";
    cout << "Enter your choice: ";
    cin >> paymentChoice;

    switch (paymentChoice) {
        case 1:
            cout << "\nProcessing payment via Credit Card...\n";
            break;
        case 2:
            cout << "\nProcessing payment via Debit Card...\n";
            break;
        case 3:
            cout << "\nProcessing payment in Cash...\n";
            break;
        case 4:
            cout << "\nProcessing payment via Digital Wallet...\n";
            break;
        default:
            cout << "\nInvalid payment method. Please try again.\n";
            return;
    }

    // Confirm before completing checkout
    char confirm;
    cout << "\nDo you confirm the checkout? (y/n): ";
    cin >> confirm;
    if (confirm == 'y' || confirm == 'Y') {
        cout << "\nThank you for your purchase! Your final total is " << finalTotal << " Rupees.\n";
    } else {
        cout << "\nCheckout canceled. Returning to the main menu.\n";
        return;
    }

    // Feedback collection
    char provideFeedback;
    cout << "\nWould you like to provide feedback about your shopping experience? (y/n): ";
    cin >> provideFeedback;

    if (provideFeedback == 'y' || provideFeedback == 'Y') {
        cin.ignore(); // Clear input buffer
        string feedback;
        cout << "\nPlease enter your feedback: ";
        getline(cin, feedback);
        ofstream feedbackFile("feedback.txt", ios::app);
        if (!feedbackFile) {
            cerr << "Failed to save feedback.\n";
        } else {
            feedbackFile << feedback << endl;
            cout << "Thank you for your feedback! It has been saved.\n";
        }
    }
}

// Function to load products from a file
// Function to load products without using files
void loadProducts(vector<RecycledProduct>& products) {
    // Predefined default products
    cout << "\nLoading default products." << endl;
    products = {
        {"Plastic", "Recycled Plastic Bag", 25.50},
        {"Metal", "Recycled Aluminum Can", 15.75},
        {"Paper", "Recycled Notebook", 35.00},
        {"Glass", "Recycled Glass Jar", 50.00},
        {"E-Waste", "Refurbished Smartphone", 3500.00}
    };

    // Optionally, allow user to add products manually
    cout << "\nWould you like to add more products manually? (y/n): ";
    char addMore;
    cin >> addMore;

    if (addMore == 'y' || addMore == 'Y') {
        while (true) {
            RecycledProduct newProduct;
            cout << "\nEnter waste type: ";
            cin.ignore();
            getline(cin, newProduct.wasteType);

            cout << "Enter product name: ";
            getline(cin, newProduct.productName);

            cout << "Enter product price: ";
            cin >> newProduct.price;

            if (!newProduct.wasteType.empty() && !newProduct.productName.empty() && newProduct.price > 0) {
                products.push_back(newProduct);
            } else {
                cout << "\nInvalid input. Product not added." << endl;
            }

            cout << "\nAdd another product? (y/n): ";
            char continueAdding;
            cin >> continueAdding;
            if (continueAdding != 'y' && continueAdding != 'Y') {
                break;
            }
        }
    }

    // Display a summary of products
    cout << "\nSummary of Products:\n";
    cout << left << setw(5) << "No." << setw(20) << "Waste Type" << setw(25) << "Product Name" << "Price (in Rupees)" << endl;
    cout << string(60, '-') << endl;
    for (size_t i = 0; i < products.size(); ++i) {
        cout << left << setw(5) << i + 1 << setw(20) << products[i].wasteType
             << setw(25) << products[i].productName << products[i].price << endl;
    }
}



// Function to save cart to a file
void saveCart(const vector<CartItem>& cart) {
    if (cart.empty()) {
        cout << "\nYour cart is empty. Nothing to save." << endl;
        return;
    }

    // Validate cart items
    bool invalidItemFound = false;
    for (const auto& item : cart) {
        if (item.quantity <= 0 || item.product.price <= 0) {
            invalidItemFound = true;
            cout << "\nInvalid item found in the cart. Skipping: "
                 << item.product.productName << " (Quantity: " << item.quantity
                 << ", Price: " << item.product.price << ")" << endl;
        }
    }

    if (invalidItemFound) {
        cout << "\nCart contains invalid items. Only valid items will be saved." << endl;
    }

    // Open file for writing
    ofstream file("cart.txt");
    if (!file) {
        cerr << "\nFailed to open file for saving the cart." << endl;
        return;
    }

    // Write cart header
    file << left << setw(25) << "Product Name" << setw(10) << "Quantity"
         << setw(15) << "Price (Rupees)" << setw(15) << "Total Price (Rupees)" << endl;
    file << string(65, '-') << endl;

    // Write each cart item
    double grandTotal = 0.0;
    for (const auto& item : cart) {
        if (item.quantity > 0 && item.product.price > 0) {
            double totalPrice = item.quantity * item.product.price;
            grandTotal += totalPrice;

            file << left << setw(25) << item.product.productName
                 << setw(10) << item.quantity
                 << setw(15) << item.product.price
                 << setw(15) << totalPrice << endl;
        }
    }

    // Write grand total
    file << string(65, '-') << endl;
    file << left << setw(50) << "Grand Total" << grandTotal << endl;

    // Log save operation
    cout << "\nCart successfully saved to 'cart.txt'." << endl;
    cout << "Summary of saved cart:" << endl;

    for (const auto& item : cart) {
        if (item.quantity > 0 && item.product.price > 0) {
            double totalPrice = item.quantity * item.product.price;
            cout << "Product: " << item.product.productName
                 << ", Quantity: " << item.quantity
                 << ", Total Price: " << totalPrice << " Rupees" << endl;
        }
    }

    cout << "Grand Total: " << grandTotal << " Rupees" << endl;
}
// Function to setup the graph
void setupGraph(ProductGraph& graph, const vector<RecycledProduct>& products) {
    for (const auto& product : products) {
        graph.addEdge(product.wasteType, product.productName);
    }
}
void loadCart(vector<CartItem>& cart, const vector<RecycledProduct>& products) {
    ifstream inFile("cart.txt"); // Open the file to read cart details
    if (!inFile) {
        cerr << "Error: Could not open file to load cart details.\n";
        return;
    }

    cart.clear(); // Clear existing cart data before loading new data

    string line;
    getline(inFile, line); // Skip header line
    getline(inFile, line); // Skip separator line

    while (getline(inFile, line)) {
        istringstream iss(line);
        string productName;
        int quantity;
        double price;

        // Read values from the line
        if (iss >> quoted(productName) >> quantity >> price) {
            // Find the corresponding product in the existing products
            auto it = find_if(products.begin(), products.end(), [&](const RecycledProduct& p) {
                return p.productName == productName && p.price == price;
            });

            if (it != products.end()) {
                cart.push_back({*it, quantity});
            } else {
                cerr << "Warning: Product '" << productName << "' not found in available products. Skipping.\n";
            }
        } else {
            cerr << "Error: Malformed line in cart file: " << line << '\n';
        }
    }

    inFile.close();
    cout << "Cart details loaded successfully from cart.txt.\n";
}

// Enhanced Error Handling
bool isValidWasteType(const string& wasteType, const vector<RecycledProduct>& products) {
    for (const auto& product : products) {
        if (product.wasteType == wasteType) return true;
    }
    return false;
}

void suggestWasteType(const vector<RecycledProduct>& products) {
    set<string> wasteTypes;
    for (const auto& product : products) {
        wasteTypes.insert(product.wasteType);
    }
    cout << "\nSuggested Waste Types: ";
    for (const auto& wasteType : wasteTypes) {
        cout << wasteType << ", ";
    }
    cout << endl;
}

// Waste-Type Connectivity Checker
bool isConnected(const ProductGraph& graph, const string& waste1, const string& waste2) {
    vector<string> neighbors = graph.getRelatedProducts(waste1);
    return find(neighbors.begin(), neighbors.end(), waste2) != neighbors.end();

}

void selectCompany() {
    cout << "\nSelect the type of company you want to buy from:" << endl;
    cout << "1. Local Small Business" << endl;
    cout << "2. National Corporation" << endl;
    cout << "3. International Brand" << endl;
    cout << "4. Eco-Friendly Certified Companies" << endl;
    cout << "Enter your choice: ";

    int companyType;
    cin >> companyType;

    switch (companyType) {
        case 1:
            cout << "\nYou chose to buy from Local Small Businesses." << endl;
            cout << "Please select a company from the list below:" << endl;
            cout << "1. Green Planet Supplies" << endl;
            cout << "2. EcoFriendly Hub" << endl;
            cout << "3. Recycle & Renew Co." << endl;
            break;
        case 2:
            cout << "\nYou chose to buy from National Corporations." << endl;
            cout << "Please select a company from the list below:" << endl;
            cout << "1. National EcoGoods" << endl;
            cout << "2. RecycleCorp Ltd." << endl;
            cout << "3. CleanEarth Solutions" << endl;
            break;
        case 3:
            cout << "\nYou chose to buy from International Brands." << endl;
            cout << "Please select a company from the list below:" << endl;
            cout << "1. Global Recycle Inc." << endl;
            cout << "2. EcoWorld Enterprises" << endl;
            cout << "3. Renewables Unlimited" << endl;
            break;
        case 4:
            cout << "\nYou chose to buy from Eco-Friendly Certified Companies." << endl;
            cout << "Please select a company from the list below:" << endl;
            cout << "1. Certified Green Co." << endl;
            cout << "2. PlanetSafe Supplies" << endl;
            cout << "3. EcoSeal Enterprises" << endl;
            break;
        default:
            cout << "\nInvalid choice. Please select a valid company type." << endl;
    }

    cout << "Enter your company choice (1-3): ";
    int companyChoice;
    cin >> companyChoice;

    switch (companyChoice)
    {
        case 1:
            cout << "\nYou selected the first company." << endl;
            break;
        case 2:
            cout << "\nYou selected the second company." << endl;
            break;
        case 3:
            cout << "\nYou selected the third company." << endl;
            break;
        default:
            cout << "\nInvalid company choice." << endl;
    }
}

int sellrecycled() {
    vector<RecycledProduct> products;
    vector<CartItem> cart;

    // Save the cart to a file
    saveCart(cart);


    ProductGraph graph;



    if (products.empty()) {
        // If no products are loaded, use default values
        cout << ".\n";
        products = {
            {"Plastic", "Recycled Plastic Bag", 25.50},
            {"Metal", "Recycled Aluminum Can", 15.75},
            {"Paper", "Recycled Notebook", 35.00},
            {"Glass", "Recycled Glass Jar", 50.00},
            {"E-Waste", "Refurbished Smartphone", 3500.00}
        };
    }


    while (true) {
        cout << "\nMenu:\n";
        cout << "1. Display Products\n";
        cout << "2. Select Product\n";
        cout << "3. View Cart\n";
        cout << "4. Checkout\n";
        cout << "5. Save Cart\n";
        cout<< "6.load cart\n";
        cout << "7. View Waste-Product Relationships\n";
        cout << "8. Select Company\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        if (cin.fail()) {
            cin.clear(); // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            cout << "\nInvalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1:
                displayProducts(products);
                break;
            case 2:
                selectProduct(products, cart);
                break;
            case 3:
                displayCart(cart);
                break;
            case 4:
                checkout(cart);
                cart.clear(); // Empty the cart after checkout
                break;
            case 5:
                saveCart(cart);
                break;
            case 6:
                loadCart(cart,products);
                break;
            case 7:
                graph.displayGraph();
                break;

            case 8:
                selectCompany();
                break;
            case 9:
                cout << "\nExiting program. Goodbye!\n";
                return 0;
            default:
                cout << "\nInvalid choice. Please try again.\n";
        }

    }
   return 0;
}


// Route Management

#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <string>
#include <stack>
#include <map>
#include <regex>
#include <fstream>
#include <algorithm>


class Vehicle
{
public:
    std::string vehicleModel;
    int loadCapacity;
    bool isAllotted;
    double mileage;
     // Mileage in km/l

    Vehicle(std::string model, int capacity, double mileage)
     {

        vehicleModel = model;
        loadCapacity = capacity;
        isAllotted = false;
        this->mileage = mileage;
    }

};


class Driver
 {

public:
    std::string name;
    int age;
    std::string address;
    bool isAllotted;

    Driver(std::string n, int a, std::string addr)
     {
        name = n;
        age = a;
        address = addr;
        isAllotted = false;
    }

};


class Waste
{

public:
    std::string area;
    int quantity;

    Waste(std::string a, int q)
     {
        area = a;
        quantity = q;
    }

};


// Struct to hold allocation details

struct AllocationDetail
{

    std::string vehicleModel;
    std::string driverName;
    std::string wasteArea;
    double fuelRequired;
    // Added route
    std::vector<std::string> route;

};


class BelgaumWasteManagement
{

private:
    std::vector<Vehicle> vehicles;
    std::vector<Driver> drivers;
    std::vector<Waste> wastes;
    // Graph for routes
    std::vector<std::vector<std::pair<int, int>>> graph;
    // Areas for destination
    std::vector<std::string> areas;
    // Map to store allocations by date
    std::map<std::string, std::vector<AllocationDetail>> allocations;



public:
    BelgaumWasteManagement()

    {
        // Initialize areas and graph
        areas =
         {

            "Kanabargi", "Belgaum", "Shivaji Nagar", "Tilakwadi", "Chennamma Nagar",

            "Gandhinagar", "APMC Yard", "City Market", "Fort Road", "Engg College Road",

            "Khanapur Road", "Udyambag", "Bogarves", "Goaves", "Hindwadi",

            "Mache", "Malmaruti", "Nemgoa", "Shahpur", "Sambhaji Nagar",

            "Angol", "Balekundri", "Camp", "Dharamnath", "Fort Lake"

        };


        graph =
         {

           // Kanabargi
            {{1, 10}, {2, 15}, {4, 20}},

            // Belgaum
            {{0, 10}, {3, 25}, {5, 15}},

            // Shivaji Nagar
            {{0, 15}, {3, 10}, {6, 20}},

             // Tilakwadi
            {{1, 25}, {2, 10}, {7, 30}},

            // Chennamma Nagar
            {{0, 20}, {5, 25}, {8, 15}},

             // Gandhinagar
            {{1, 15}, {4, 25}, {9, 10}},

             // APMC Yard
            {{2, 20}, {10, 25}},

            // City Market
            {{3, 30}, {11, 15}},

              // Fort Road
            {{4, 15}, {12, 20}},

             // Engg College Road
            {{5, 10}, {13, 30}},

             // Khanapur Road
            {{6, 25}, {14, 35}},

             // Udyambag
            {{7, 15}, {15, 20}},

              // Bogarves
            {{8, 20}, {16, 10}},

              // Goaves
            {{9, 30}, {17, 25}},

              // Hindwadi
            {{10, 35}, {18, 15}},

              // Mache
            {{11, 20}, {19, 10}},

            // Malmaruti
            {{12, 10}, {20, 30}},

            // Nemgoa
            {{13, 25}, {21, 15}},

            // Shahpur
            {{14, 15}, {22, 20}},

            // Sambhaji Nagar
            {{15, 10}, {23, 25}},

              // Angol
            {{16, 30}, {24, 20}},

             // Balekundri
            {{17, 15}},

              // Camp
            {{18, 20}},

              // Dharamnath
            {{19, 25}},

                // Fort Lake
            {{20, 20}}

        };

    }

    // Function to get the index of an area
    int getAreaIndex(const std::string& areaName)
     {

        for (int i = 0; i < areas.size(); i++)
          {
            if (areas[i] == areaName)
            {
                return i;
            }
        }
        // Return -1 if area is not found
        return -1;
    }


    // Add Vehicle
    void addVehicle(Vehicle v)
    {
        vehicles.push_back(v);
    }


    // Add Driver
    void addDriver(Driver d)
    {

        drivers.push_back(d);
    }


    // Add waste
    void addWaste(Waste w)
     {
        wastes.push_back(w);
    }


    // Assuming constant speed in km/h for simplicity (40 km/h)
    // average

    const double VEHICLE_SPEED = 40.0;


    // Function to calculate travel time (in hours) based on distance and speed
    double calculateTravelTime(double distance)
    {
        return distance / VEHICLE_SPEED;
    }


// Dijkstra's Algorithm for finding the shortest path from a source to a destination in a weighted graph
std::vector<int> dijkstra(int source, int destination, std::vector<int>& parent)
{

    // Initialize the number of nodes in the graph
    int n = graph.size();

    // Initialize distance vector with maximum possible values (infinity) to indicate that initially all nodes are unreachable
    std::vector<int> distance(n, std::numeric_limits<int>::max());

    // Initialize the parent vector to store the path information
    parent.assign(n, -1);

    // Distance to the source itself is always 0
    distance[source] = 0;

    // Min-heap priority queue to select the node with the smallest distance
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> pq;
    pq.push({0, source});

    // Continue until all nodes processed or priority queue is empty
    while (!pq.empty())

    {
        // Extract the node with the smallest distance
        auto [dist, node] = pq.top();
        pq.pop();

        // If the current distance is greater than the recorded distance, skip processing
        if (dist > distance[node]) continue;

        // Explore all the neighbors of the current node
        for (const auto& [neighbor, weight] : graph[node])
        {

            // Calculate new distance for the neighbor
            int newDist = distance[node] + weight;

            // If the new distance is shorter, update the distance and parent
            if (newDist < distance[neighbor])
            {
                distance[neighbor] = newDist;
                parent[neighbor] = node;

                // Push the updated distance and neighbor node into the priority queue
                pq.push({newDist, neighbor});
            }

        }

    }

    // Check if the destination is reachable
    if (distance[destination] == std::numeric_limits<int>::max())
    {
        std::cout << "No path exists from " << areas[source] << " to " << areas[destination] << "." << std::endl;
        return {};
    }

    // Return the distance vector containing shortest distances from the source to all nodes
    return distance;
}


// Function to find the longest path using modified DFS (Depth-First Search)
void findLongestPath(int source, std::vector<int>& dist, std::vector<int>& parent)
{

    // Initialize the visited vector to keep track of visited nodes
    std::vector<bool> visited(areas.size(), false);

    // Initialize the distance vector with the minimum possible values (negative infinity)
    dist.assign(areas.size(), std::numeric_limits<int>::min());

    // Initialize the parent vector to keep track of the path
    parent.assign(areas.size(), -1);

    // Distance to the source itself is always 0
    dist[source] = 0;

    // Perform DFS from each node to find the longest path
    for (int i = 0; i < areas.size(); i++)
    {
        // Start DFS from unvisited nodes
        if (!visited[i])
        {
            dfsLongestPath(i, visited, dist, parent);
        }
    }

}


// Helper function for DFS to find the longest path
void dfsLongestPath(int u, std::vector<bool>& visited, std::vector<int>& dist, std::vector<int>& parent)
{
    // Mark the current node as visited
    visited[u] = true;

    // Explore all adjacent nodes (neighbors)
    for (const auto& [v, weight] : graph[u])
    {
        // Check if the path through the current node u is longer
        if (dist[u] + weight > dist[v])
        {
            // Update the distance and parent for the neighbor node
            dist[v] = dist[u] + weight;
            parent[v] = u;
        }

        // Perform DFS on unvisited neighbors
        if (!visited[v])
        {
            dfsLongestPath(v, visited, dist, parent);
        }
    }
}


    // Function to validate date format (dd/mm/yyyy)
  bool isValidDate(const std::string& date)
{
    // Define a regular expression pattern to match dates in the format dd/mm/yyyy
    // \b asserts a word boundary to ensure the date is a complete word
    // (0[1-9]|[12][0-9]|3[01]) matches the day part, allowing values from 01 to 31
    // (0[1-9]|1[0-2]) matches the month part, allowing values from 01 to 12
    // ([0-9]{4}) matches the year part, allowing any four-digit number
    const std::regex pattern(R"(\b(0[1-9]|[12][0-9]|3[01])/(0[1-9]|1[0-2])/([0-9]{4})\b)");

    // Use std::regex_match to check if the input date matches the pattern
    return std::regex_match(date, pattern);

}


    // Function to allot a vehicle for a waste collection task
    void allotVehicle(int wasteIndex)
    {

        if (wasteIndex < wastes.size())
          {
            int requiredCapacity = wastes[wasteIndex].quantity;
            std::string date;
            std::cout << "Enter the date for allotment (dd/mm/yyyy): ";
            std::cin >> date;
            while (!isValidDate(date))
             {

                std::cout << "Invalid date format. Please enter the date in the format dd/mm/yyyy: ";
                std::cin >> date;
            }

            for (int i = 0; i < vehicles.size(); i++)
              {
                if (vehicles[i].loadCapacity >= requiredCapacity && !vehicles[i].isAllotted)
                {
                    vehicles[i].isAllotted = true;
             // Assuming first driver is allotted for simplicity
                    drivers[0].isAllotted = true;
                    std::cout << "Vehicle allotted : " << vehicles[i].vehicleModel << std::endl;
                    std::cout << "  Driver name : " << drivers[0].name << " from " << drivers[0].address
                              << " for waste in " << wastes[wasteIndex].area << " on " << date << std::endl;
                    std::cout << " Driver address :" << drivers[0].address << std::endl;
                    std::cout << "Route details from Kanabargi to " << wastes[wasteIndex].area << ":\n";
                    int source = 0;  // Kanabargi
                    int destination = getAreaIndex(wastes[wasteIndex].area);

                    if (destination != -1)

                      {
                        std::vector<int> parent;
                        std::vector<int> distance = dijkstra(source, destination, parent);
                        if (distance[destination] == std::numeric_limits<int>::max())

                        {
                            std::cout << "No route found to the destination." << std::endl;
                            return;
                        }

                        std::stack<int> path;
                        std::vector<std::string> route;
                        for (int at = destination; at != -1; at = parent[at])

                          {
                            path.push(at);
                        }
                        while (!path.empty())

                        {
                            std::cout << areas[path.top()] << (path.size() > 1 ? " -> " : "");
                            route.push_back(areas[path.top()]);
                            path.pop();

                        }
                        std::cout << std::endl;

                        double totalDistance = distance[destination];
                        double travelTime = calculateTravelTime(totalDistance);
                        int hours = static_cast<int>(travelTime);
                        int minutes = static_cast<int>((travelTime - hours) * 60);

                        std::cout << "Total Distance: " << totalDistance << " km\n";

                        std::cout << "This vehicle will be free in " << hours << " hours and " << minutes << " minutes." << std::endl;

                        double fuelRequired = totalDistance / vehicles[i].mileage;

                        std::cout << "Fuel Required: " << fuelRequired << " liters\n";

                        allocations[date].push_back({vehicles[i].vehicleModel, drivers[0].name, wastes[wasteIndex].area, fuelRequired, route});

                        std::cout << " =========================================================" << std::endl;

                    }
                    else
                      {
                        std::cout << "Error: Invalid destination area." << std::endl;
                    }

                    return;
                }
            }

            std::cout << "No suitable vehicle available." << std::endl;
        }
        else
          {
            std::cout << "Invalid waste index." << std::endl;
        }

    }



void displayCollectedWasteByDate() {
    // Check if there are any waste collection records
    if (allocations.empty()) {
        std::cout << "No waste collection details available.\n";
        return;
    }

    std::cout << "=========================================================\n";
    std::cout << "             Waste Collection Details by Date            \n";
    std::cout << "=========================================================\n";

    // Iterate through each allocation date and its details
    for (const auto& [date, details] : allocations) {
        std::cout << "\nDate: " << date << "\n";
        std::cout << "---------------------------------------------------------\n";

        // Display all details for a specific date
        for (const auto& detail : details) {
            std::cout << "  Vehicle Model    : " << detail.vehicleModel << "\n";
            std::cout << "  Driver Name      : " << detail.driverName << "\n";
            std::cout << "  Waste Area       : " << detail.wasteArea << "\n";
            std::cout << "  Fuel Required    : " << detail.fuelRequired << " liters\n";
            std::cout << "  Route            : ";

            // Print the route with arrows between areas
            for (size_t i = 0; i < detail.route.size(); ++i) {
                std::cout << detail.route[i];
                if (i != detail.route.size() - 1) {
                    std::cout << " -> ";
                }
            }
            std::cout << "\n---------------------------------------------------------\n";
        }

        std::cout << "\n=========================================================\n";
    }

    // Summary or concluding message (optional)
    std::cout << "End of waste collection records.\n";
}

void storeDataInFile()
{

    // Hardcoded file path
    std::string filename = "C:\\Users\\mahes\\OneDrive\\Desktop\\route management\\dateDATA.txt";

    // Attempt to open the file for writing
    std::ofstream outFile(filename, std::ios::out | std::ios::app);
     // Open in append mode
    if (!outFile)
      {
        std::cerr << "Error opening file for writing." << std::endl;
        return;
    }

    // Write the header to the file
    outFile << "Collected Waste Data by Date:\n";

    // Check if there are any allocations
    if (allocations.empty())
      {
        outFile << "No waste collection data available.\n";
        std::cout << "No data to store in the file.\n";
        outFile.close(); // Close the file before returning
        return;
    }

    // Iterate over all waste collection details by date
    for (const auto& [date, details] : allocations)
      {

        outFile << "Date: " << date << "\n";
        for (const auto& detail : details)
        {

            outFile << "Vehicle Model: " << detail.vehicleModel
                    << ", Driver Name: " << detail.driverName
                    << ", Waste Area: " << detail.wasteArea
                    << ", Fuel Required: " << detail.fuelRequired << " liters\n";
            outFile << "Route: ";

            for (const auto& area : detail.route)
              {
                outFile << area << (area == detail.route.back() ? "" : " -> ");
            }
            outFile << "\n";
        }

        outFile << " ---------------------------------------------------------\n";
    }

    // Close the file and notify the user
    outFile.close();
    std::cout << "Data stored successfully in " << filename << std::endl;

}

//display the content of file

void displayFileContent(const std::string& filename)
{

    // Attempt to open the file for reading
    std::ifstream inFile(filename);
    if (!inFile)
      {
        std::cerr << "Error: Unable to open file '" << filename << "' for reading.\n";
        return;
    }

    // Display a header for better clarity
    std::cout << "=========================================================\n";
    std::cout << "                 Displaying File Content                 \n";
    std::cout << "=========================================================\n";

    // Read and display file content line by line

    std::string line;
    bool isEmpty = true;
    while (std::getline(inFile, line))

      {
        isEmpty = false;
        std::cout << line << "\n";
    }

    // Check if the file was empty
    if (isEmpty)
      {
        std::cout << "The file is empty.\n";
    }

    // Footer for better output structure
    std::cout << "=========================================================\n";
    std::cout << "                  End of File Content                    \n";
    std::cout << "=========================================================\n";

    // Close the file
    inFile.close();

}

//load content of file
// display allocation record

void loadDataFromFile(const std::string& filename)
 {
    // Attempt to open the file for reading
    std::ifstream inFile(filename);
    if (!inFile)
      {
        std::cerr << "Error: Unable to open file '" << filename << "' for reading.\n";
        return;

    }

    // Display a header for better clarity
    std::cout << "=========================================================\n";
    std::cout << "                 Displaying File Content                 \n";
    std::cout << "=========================================================\n";

    // Read and display file content line by line
    std::string line;
    bool isEmpty = true;
    while (std::getline(inFile, line))
      {

        isEmpty = false;
        std::cout << line << "\n";
    }

    // Check if the file was empty
    if (isEmpty)
      {

        std::cout << "The file is empty.\n";
    }

    // Footer for better output structure
    std::cout << "=========================================================\n";
    std::cout << "                  End of File Content                    \n";
    std::cout << "=========================================================\n";

    // Close the file
    inFile.close();

}

// diplay Menu
    void displayMenu()
     {

        std::cout << " --------------------------------------------------------------------------------------------------------" << std::endl;
        std::cout << "    " << std::endl;
        std::cout << "Belgaum City Waste Management System" << std::endl;
        std::cout << "1. Allot Vehicle" << std::endl;
        std::cout << "2. Display Vehicles" << std::endl;
        std::cout << "3. Display Drivers" << std::endl;
        std::cout << "4. Display Wastes" << std::endl;
        std::cout << "5. Shortest Route Details" << std::endl;
        std::cout << "6. Longest Route Details" << std::endl;
        std::cout << "7. View Available Vehicles and Drivers" << std::endl;
        std::cout << "8. Display Collected Waste by Date" << std::endl;
        std::cout << "9. Save Collected Waste Data to File" << std::endl;
        std::cout << "10. Load Data from File" << std::endl;
        std::cout << "11. Exit" << std::endl;
        std::cout << " ---------------------------------------------------------------------------------------------------------" << std::endl;


    }


void viewAvailableVehiclesAndDrivers()
{

    // Display header for available vehicles
    std::cout << "=========================================================\n";
    std::cout << "                   Available Vehicles                   \n";
    std::cout << "=========================================================\n";

    bool foundAvailableVehicle = false;
    for (size_t i = 0; i < vehicles.size(); i++)
      {

        if (!vehicles[i].isAllotted)
        {
            std::cout << "Vehicle Model: " << vehicles[i].vehicleModel << "\n"
                      << "  Load Capacity: " << vehicles[i].loadCapacity << " kg\n"
                      << "  Mileage: " << vehicles[i].mileage << " km/l\n";
            std::cout << "---------------------------------------------------------\n";
            foundAvailableVehicle = true;
        }
    }

    if (!foundAvailableVehicle)
      {
        std::cout << "No available vehicles at the moment.\n";
    }

    // Display header for available drivers
    std::cout << "\n=========================================================\n";
    std::cout << "                   Available Drivers                    \n";
    std::cout << "=========================================================\n";


    bool foundAvailableDriver = false;
    for (size_t i = 0; i < drivers.size(); i++)
      {
        if (!drivers[i].isAllotted)
          {

            std::cout << "Driver Name: " << drivers[i].name << "\n"
                      << "  Age: " << drivers[i].age << "\n"
                      << "  Address: " << drivers[i].address << "\n";
            std::cout << "---------------------------------------------------------\n";
            foundAvailableDriver = true;
        }
    }

    if (!foundAvailableDriver)
      {

        std::cout << "No available drivers at the moment.\n";
    }

    // Footer for better structure
    std::cout << "=========================================================\n";
    std::cout << "                  End of Availability                   \n";
    std::cout << "=========================================================\n";

}

void displayVehicles()
{

    // Display header for the vehicles section
    std::cout << "=========================================================\n";
    std::cout << "                    List of Vehicles                    \n";
    std::cout << "=========================================================\n";

    if (vehicles.empty())
      {

        std::cout << "No vehicles available in the system.\n";
    }
    else
      {

        for (int i = 0; i < vehicles.size(); i++)

        {
            std::cout << "---------------------------------------------------------\n";
            std::cout << "Vehicle " << i + 1 << ":\n";
            std::cout << "  Model: " << vehicles[i].vehicleModel << "\n"
                      << "  Load Capacity: " << vehicles[i].loadCapacity << " kg\n"
                      << "  Mileage: " << vehicles[i].mileage << " km/l\n"
                      << "  Allotted: " << (vehicles[i].isAllotted ? "Yes" : "No") << "\n";
            std::cout << "---------------------------------------------------------\n";
        }

    }

    // Display footer for the vehicles section
    std::cout << "=========================================================\n";
    std::cout << "                  End of Vehicle List                   \n";
    std::cout << "=========================================================\n";
}


void displayDrivers()
{
    // Display header for the drivers section
    std::cout << "=========================================================\n";
    std::cout << "                    List of Drivers                     \n";
    std::cout << "=========================================================\n";

    if (drivers.empty())
      {

        std::cout << "No drivers available in the system.\n";
    }
    else
      {
        for (int i = 0; i < drivers.size(); i++)
         {
            std::cout << "---------------------------------------------------------\n";
            std::cout << "Driver " << i + 1 << ":\n";
            std::cout << "  Name: " << drivers[i].name << "\n"
                      << "  Age: " << drivers[i].age << " years\n"
                      << "  Address: " << drivers[i].address << "\n";
            std::cout << "---------------------------------------------------------\n";
        }

    }

    // Display footer for the drivers section
    std::cout << "=========================================================\n";
    std::cout << "                  End of Driver List                    \n";
    std::cout << "=========================================================\n";

}

void displayWastes()
{

    // Display header for the wastes section
    std::cout << "=========================================================\n";
    std::cout << "                    Waste Information                    \n";
    std::cout << "=========================================================\n";

    if (wastes.empty())
      {

        std::cout << "No waste data available.\n";
    }
    else
      {
        for (int i = 0; i < wastes.size(); i++)
          {
            std::cout << "---------------------------------------------------------\n";
            std::cout << "Waste " << i + 1 << ":\n";
            std::cout << "  Area: " << wastes[i].area << "\n"
                      << "  Quantity: " << wastes[i].quantity << " kg\n";
            std::cout << "---------------------------------------------------------\n";
        }

    }

    // Display footer for the wastes section
    std::cout << "=========================================================\n";
    std::cout << "                   End of Waste List                    \n";
    std::cout << "=========================================================\n";
}


void findLongestRouteToDestination(int source, int destination)
{

    // Vector to store parent nodes and distances from the source node

    std::vector<int> parent;
    std::vector<int> dist;

    // Call findLongestPath to compute the longest path from the source

    findLongestPath(source, dist, parent);

    // Check if the destination is reachable from the source
    if (dist[destination] == std::numeric_limits<int>::min())
      {

        std::cout << "No path exists from " << areas[source] << " to " << areas[destination] << "." << std::endl;
        return;
    }

    // Display the longest distance to the destination
    std::cout << "Longest distance to " << areas[destination] << ": " << dist[destination] << " km" << std::endl;

    // Reconstruct and display the longest path

}

 // Existing code...

    // to simulate vehicle maintenance
    void performVehicleMaintenance()
    {
        // Placeholder for maintenance logic
    }

    //  to simulate driver training
    void conductDriverTraining()
    {
        // Placeholder for training logic
    }

    //  to simulate waste segregation
    void segregateWaste()
    {
        // Placeholder for waste segregation logic
    }

    //  simulate fuel efficiency calculation
    void calculateFuelEfficiency()
    {
        // Placeholder for fuel efficiency calculation logic
    }

    // to simulate route optimization
    void optimizeRoutes()
    {
        // Placeholder for route optimization logic
    }

    // to simulate waste disposal reporting
    void generateWasteDisposalReport()
    {
        // Placeholder for report generation logic
    }

    //  to simulate emergency response
    void handleEmergencySituation()
    {
        // Placeholder for emergency handling logic
    }

    //  to simulate vehicle tracking
    void trackVehicles()
    {
        // Placeholder for vehicle tracking logic
    }

    // to simulate driver feedback collection
    void collectDriverFeedback()
    {
        // Placeholder for feedback collection logic
    }

    // to simulate waste collection scheduling
    void scheduleWasteCollection()
    {
        // Placeholder for scheduling logic
    }

    // all remaining functions

//calculate shortest route
// use of dijkastra algorithm
void findShortestRouteToDestination(int source, int destination)
{
    std::vector<int> parent;

    // Compute the shortest distances and parent paths using Dijkstra's algorithm
    auto distances = dijkstra(source, destination, parent);

    // Check if the destination is reachable from the source
    if (distances[destination] == std::numeric_limits<int>::max())
      {

        std::cout << "No path exists from " << areas[source] << " to " << areas[destination] << "." << std::endl;
        return;
    }

    // Display the shortest distance to the destination
    std::cout << "Shortest distance to " << areas[destination] << ": " << distances[destination] << " km" << std::endl;

    // Display the shortest route from source to destination
    std::cout << "Route: ";
    std::stack<int> path;
    for (int at = destination; at != -1; at = parent[at])
      {
        path.push(at);
    }

    // Output the path from source to destination in human-readable format
    while (!path.empty())
    {

        std::cout << areas[path.top()] << (path.size() > 1 ? " -> " : "");
        path.pop();
    }

    std::cout << std::endl;

    // Calculate and display additional travel information
    //in hr and min
    double totalDistance = distances[destination];
    double travelTime = calculateTravelTime(totalDistance);

    // Extract hours and minutes from the travel time
    int hours = static_cast<int>(travelTime);
    int minutes = static_cast<int>((travelTime - hours) * 60);

    // Display the travel time and other relevant information
    std::cout << " =========================================================" << std::endl;
    std::cout << "Total Distance: " << totalDistance << " km\n";

    // Display the estimated travel time in hours and minutes
    std::cout << "Estimated Travel Time: " << hours << " hours and " << minutes << " minutes." << std::endl;

    // Additional informative output for the user
    std::cout << "\nPlease ensure the vehicle is ready for travel and has sufficient fuel." << std::endl;
    std::cout << "Remember to check the weather and road conditions before starting the journey." << std::endl;

    // Conclusion message
    std::cout << " =========================================================" << std::endl;

}

};


int route()

{
    BelgaumWasteManagement bwm;

    // Add Vehicles
    //vehicle1
    bwm.addVehicle(Vehicle("Tata Ace", 1000, 13.0));
    //vehicle2
    bwm.addVehicle(Vehicle("Mahindra Pickup", 2000, 12.0));
    //vehicle3
    bwm.addVehicle(Vehicle("Ashok Leyland Dost", 3000, 14.0));
    //vehicle4
    bwm.addVehicle(Vehicle("BharatBenz 1160", 4000, 11.0));
    //vehicle5
    bwm.addVehicle(Vehicle("Eicher 950", 6000, 12.0));
    //vehicle6
    bwm.addVehicle(Vehicle("Tata Ace Hybrid", 9000, 11.0));
    //vehicle7
    bwm.addVehicle(Vehicle("Tata Force", 10000, 10.0));
    //vehicle8
    bwm.addVehicle(Vehicle("Leyland Bada Dost", 11000, 9.0));
    //vehicle9
    bwm.addVehicle(Vehicle("MAN 1600", 12000, 8.0));
    //vehicle10
    bwm.addVehicle(Vehicle("Tata 1630", 14000, 7.0));
    //All vehicle


    // Add Drivers



    // All drivers

    bwm.addDriver(Driver("Abhi", 30, "Gokak"));
    bwm.addDriver(Driver("Arun", 32, "Hubli"));
    bwm.addDriver(Driver("Akash", 25, "Kanabargi"));
    bwm.addDriver(Driver("Gagan", 28, "Sambre"));
    bwm.addDriver(Driver("Venki", 26, "Suldal"));

    // Add Wastes
    //wastearea1
    bwm.addWaste(Waste("Kanabargi", 500));

    //wastearea2
    bwm.addWaste(Waste("BelgaumCBT", 1000));


    //wastearea3
    bwm.addWaste(Waste("Shivaji Nagar", 1500));
    //wastearea4
    bwm.addWaste(Waste("Tilakwadi", 2000));

    //wastearea5
    bwm.addWaste(Waste("Chennamma Nagar", 2500));

    //wastearea6
    bwm.addWaste(Waste("Gandhinagar", 3000));

    //wastearea7
    bwm.addWaste(Waste("APMC Yard", 3500));

    //wastearea8
    bwm.addWaste(Waste("City Market", 4000));

    //wastearea9
    bwm.addWaste(Waste("Fort Road", 4500));

    //wastearea10
    bwm.addWaste(Waste("Engg College Road", 5000));

    //wastearea11
    bwm.addWaste(Waste("Khanapur Road", 5500));

    //wastearea12
    bwm.addWaste(Waste("Udyambag", 6000));

    //wastearea13
    bwm.addWaste(Waste("Bogarves", 6500));


    //wastearea14
    bwm.addWaste(Waste("Goaves", 7000));

    //wastearea15
    bwm.addWaste(Waste("Hindwadi", 7500));

    //wastearea16
    bwm.addWaste(Waste("Mache", 8000));

    //wastearea17
    bwm.addWaste(Waste("Malmaruti", 8500));

    //wastearea18
    bwm.addWaste(Waste("Nemgoa", 9000));

    //wastearea19
    bwm.addWaste(Waste("Shahpur", 9500));

    //wastearea20
    bwm.addWaste(Waste("Sambhaji Nagar", 10000));

    //wastearea21
    bwm.addWaste(Waste("Angol", 10500));

    //wastearea22
    bwm.addWaste(Waste("Balekundri", 11000));

    //wastearea23
    bwm.addWaste(Waste("Camp", 11500));
    //wastearea24
    bwm.addWaste(Waste("Dharamnath", 12000));

    //wastearea25
    bwm.addWaste(Waste("Fort Lake", 12500));


    int choice;

    do
      {

        bwm.displayMenu();
        std::cout << "Enter your choice: ";
        std::cin >> choice;


        // Handle invalid input for menu choice
        if(std::cin.fail())
          {
            std::cin.clear();
             // clear error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            // ignore invalid input
            std::cout << "Invalid input. Please enter a valid number.\n";
            continue;

        }

        switch (choice)
         {

        case 1:
          {

            int wasteIndex;
             std::cout << "Enter destination area index:\n";
              std::cout << "1: Kanabargi\n";
              std::cout << "2: Belgaum\n";
              std::cout << "3: Shivaji Nagar\n";
              std::cout << "4: Tilakwadi\n";
              std::cout << "5: Chennamma Nagar\n";
              std::cout << "6: Gandhinagar\n";
              std::cout << "7: APMC Yard\n";
              std::cout << "8: City Market\n";
              std::cout << "9: Fort Road\n";
              std::cout << "10: Engg College Road\n";
              std::cout << "11: Khanapur Road\n";
              std::cout << "12: Udyambag\n";
              std::cout << "13: Bogarves\n";
              std::cout << "14: Goaves\n";
              std::cout << "15: Hindwadi\n";
              std::cout << "16: Mache\n";
              std::cout << "17: Malmaruti\n";
              std::cout << "18: Nemgoa\n";
              std::cout << "19: Shahpur\n";
              std::cout << "20: Sambhaji Nagar\n";
              std::cout << "21: Angol\n";
              std::cout << "22: Balekundri\n";
              std::cout << "23: Camp\n";
              std::cout << "24: Dharamnath\n";
              std::cout << "25: Fort Lake\n";
            std::cin >> wasteIndex;
            if(wasteIndex < 1 || wasteIndex > 25)
              {

                std::cout << "Invalid waste index. Please enter a number between 1 and 25.\n";
            }
            else
              {

                bwm.allotVehicle(wasteIndex - 1);
            }

            break;
        }


        case 2:
            bwm.displayVehicles();
            break;


        case 3:
            bwm.displayDrivers();
            break;


        case 4:
            bwm.displayWastes();
            break;


        case 5:
           {

            int destination;
            std::cout << "Enter destination area index (1: Kanabargi, 2: Belgaum, 3: Shivaji Nagar, ... 25: Fort Lake): ";

            std::cin >> destination;

            if(destination < 1 || destination > 25)
              {

                std::cout << "Invalid destination. Please enter a number between 1 and 25.\n";

            }
             else
              {

                int source = 0; // Kanabargi
                bwm.findShortestRouteToDestination(source, destination - 1);
            }
            break;
        }


        case 6:
          {

            int destination;
           std::cout << "Enter destination area index:\n";
              std::cout << "1: Kanabargi\n";
              std::cout << "2: Belgaum\n";
              std::cout << "3: Shivaji Nagar\n";
              std::cout << "4: Tilakwadi\n";
              std::cout << "5: Chennamma Nagar\n";
              std::cout << "6: Gandhinagar\n";
              std::cout << "7: APMC Yard\n";
              std::cout << "8: City Market\n";
              std::cout << "9: Fort Road\n";
              std::cout << "10: Engg College Road\n";
              std::cout << "11: Khanapur Road\n";
              std::cout << "12: Udyambag\n";
              std::cout << "13: Bogarves\n";
              std::cout << "14: Goaves\n";
              std::cout << "15: Hindwadi\n";
              std::cout << "16: Mache\n";
              std::cout << "17: Malmaruti\n";
              std::cout << "18: Nemgoa\n";
              std::cout << "19: Shahpur\n";
              std::cout << "20: Sambhaji Nagar\n";
              std::cout << "21: Angol\n";
              std::cout << "22: Balekundri\n";
              std::cout << "23: Camp\n";
              std::cout << "24: Dharamnath\n";
              std::cout << "25: Fort Lake\n";
            std::cin >> destination;
            if(destination < 1 || destination > 25)
              {

                std::cout << "Invalid destination. Please enter a number between 1 and 25.\n";
            }
            else
              {
                int source = 0;
                // Kanabargi

                bwm.findLongestRouteToDestination(source, destination - 1);
            }

            break;
        }

        case 7:
            bwm.viewAvailableVehiclesAndDrivers();
            break;


        case 8:
            bwm.displayCollectedWasteByDate();
            break;


        case 9:
            bwm.storeDataInFile();

            break;



        case 10:

          {

            std::string filename = "C:\\Users\\mahes\\OneDrive\\Desktop\\route management\\dateDATA.txt";
            //file path


            bwm.loadDataFromFile(filename);
            break;
        }



        case 11:
            std::cout << "Exiting the program." << std::endl;
            break;

        default:

            std::cout << "Invalid choice. Please try again." << std::endl;
        }


    } while (choice != 11);


    return 0;


}


//module1
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <map>

using namespace std;

class Area
{
public:
    string name;
    string address;
    string city;

    void display() const
    {
        cout << left << setw(30) << name << setw(50) << address << setw(30) << city << endl;
    }
};

class AreaDatabase
{
private:
    vector<Area> areas;
    map<string, double> populationData;

public:
    void displayWelcomeMessage() const;
    void loadFromFile();
    void displayAllAreas() const;
    void updateAreaData();
    void deleteAreaByName();
    void addNewArea();
    void saveToFile() const;
    void enterPopulationData();
    void calculateWasteGeneration() const;
    void sortAreasByName();
    void filterAreasByCity();
    void exportToCSV() const;
    void calculateAveragePopulation() const;
    void printPopulationStats() const;
    void calculateAverageWasteGeneration() const; // New function declaration
};


void AreaDatabase::displayWelcomeMessage() const {
    for (int i = 0; i < 80; i++) cout << "*";
    cout << "\n\n\t\t\t\tAREA_INFO\n\n\n";
    for (int i = 0; i < 80; i++) cout << "*";
    cout << "\n\n";
}
void AreaDatabase::loadFromFile()
{
    string filename;
    cout << "Enter the filename to load area data from: ";
    cin >> filename;
    ifstream file(filename);

    if (!file)
    {
        cout << "Error: File \"" << filename << "\" not found.\n";
        return;
    }

    areas.clear();
    Area temp;

    int lineCount = 0;
    int errorsCount = 0;
    string line;
    while (getline(file, line))
    {
        lineCount++;
        stringstream ss(line);
        string name, address, city;

        // Try to parse the line
        if (getline(ss, name, ',') && getline(ss, address, ',') && getline(ss, city)) {
            temp.name = name;
            temp.address = address;
            temp.city = city;
            areas.push_back(temp);
        }
        else
        {
            // Handle parsing error
            cout << "Error parsing line " << lineCount << ": " << line << endl;
            errorsCount++;
        }
    }

    file.close();

    if (errorsCount == 0)
    {
        cout << "Area data successfully loaded from file \"" << filename << "\".\n";
    }
    else
    {
        cout << errorsCount << " errors occurred during file loading.\n";
        cout << "Some areas may not have been added correctly.\n";
    }

    // Display the number of lines loaded and errors encountered
    cout << lineCount << " total lines processed.\n";
    if (errorsCount > 0)
    {
        cout << errorsCount << " lines could not be parsed correctly.\n";
    }
    else
    {
        cout << "All lines were successfully parsed.\n";
    }

    // Optionally, print out the loaded areas if no errors
    if (errorsCount == 0)
    {
        cout << "Displaying all areas loaded from the file:\n";
        displayAllAreas();
    }
    else
    {
        cout << "Displaying only the correctly loaded areas:\n";
        for (const auto& area : areas)
        {
            area.display();
        }
    }

    // Ask the user if they want to reload the data or proceed
    char choice;
    cout << "Would you like to reload the data from another file (y/n)? ";
    cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        loadFromFile();
    }
     else
    {
        cout << "Proceeding with the loaded data.\n";
    }

    // Example of additional parsing logic for handling extra data fields (if applicable)
    char extraChoice;
    cout << "Would you like to load additional data from another file (such as population data)? (y/n): ";
    cin >> extraChoice;
    if (extraChoice == 'y' || extraChoice == 'Y') {
        string additionalFile;
        cout << "Enter the filename for the additional data (e.g., population data): ";
        cin >> additionalFile;

        ifstream additionalFileStream(additionalFile);
        if (!additionalFileStream) {
            cout << "Error: File \"" << additionalFile << "\" not found.\n";
            return;
        }

        // This could be logic to process additional data
        string areaName;
        double population;
        while (getline(additionalFileStream, areaName, ',') && additionalFileStream >> population) {
            // Example additional processing logic for population data
            // Assuming a map populationData exists, we can add it
            areaName.erase(remove(areaName.begin(), areaName.end(), ' '), areaName.end()); // Remove spaces
            populationData[areaName] = population;
        }

        additionalFileStream.close();
        cout << "Additional data loaded successfully from \"" << additionalFile << "\".\n";
    }

    // Adding logic to verify the consistency of data
    if (areas.size() != populationData.size()) {
        cout << "Warning: There is a mismatch between the number of areas and population data.\n";
    } else {
        cout << "Area and population data are consistent.\n";
    }

    // Provide options for validating data integrity
    char validationChoice;
    cout << "Would you like to validate the loaded data for correctness (y/n)? ";
    cin >> validationChoice;
    if (validationChoice == 'y' || validationChoice == 'Y')
    {
        cout << "Validating loaded data...\n";
        int validCount = 0;
        for (const auto& area : areas)
       {
            // Example validation: Check if the area has a valid city and address
            if (!area.name.empty() && !area.address.empty() && !area.city.empty()) {
                validCount++;
            } else {
                cout << "Invalid data detected for area: " << area.name << endl;
            }
        }

        cout << validCount << " valid areas found out of " << areas.size() << ".\n";
    }

    // Advanced option to handle file format errors more robustly
    cout << "Would you like to attempt an auto-correction for file format issues (y/n)? ";
    cin >> validationChoice;
    if (validationChoice == 'y' || validationChoice == 'Y') {
        cout << "Attempting to auto-correct file format issues...\n";
        // Advanced auto-correction logic can be added here based on the file format (e.g., handling missing commas, fixing newlines, etc.)
    }

    // After all data is loaded, provide options to save the data back to a file if needed
    char saveChoice;
    cout << "Would you like to save the loaded data to a new file (y/n)? ";
    cin >> saveChoice;
    if (saveChoice == 'y' || saveChoice == 'Y') {
        string newFileName;
        cout << "Enter the filename to save the data: ";
        cin >> newFileName;
        ofstream saveFile(newFileName);

        if (!saveFile) {
            cout << "Error: Could not open file \"" << newFileName << "\" for saving.\n";
            return;
        }

        for (const auto& area : areas) {
            saveFile << area.name << "," << area.address << "," << area.city << endl;
        }

        saveFile.close();
        cout << "Data saved successfully to \"" << newFileName << "\".\n";
    }

    // Final confirmation and summary of loaded data
    cout << "\nFinal Summary of Loaded Data:\n";
    cout << "Total Areas Loaded: " << areas.size() << endl;
    cout << "Total Population Data Entries: " << populationData.size() << endl;
    cout << "Any errors or inconsistencies were addressed.\n";
    cout << "Proceeding with program...\n";
}
void AreaDatabase::displayAllAreas() const {
    if (areas.empty())
    {
        cout << "No area data to display. Please load data first.\n";
        return;
    }

    // Sorting option for displaying areas
    char sortChoice;
    cout << "Would you like to sort the areas by name (y/n)? ";
    cin >> sortChoice;

    vector<Area> sortedAreas = areas;

    if (sortChoice == 'y' || sortChoice == 'Y')
    {
        sort(sortedAreas.begin(), sortedAreas.end(), [](const Area& a, const Area& b) {
            return a.name < b.name;
        });
        cout << "Areas sorted by name.\n";
    }

    else
    {
        cout << "Displaying areas without sorting.\n";
    }

    // Filter option to only show areas in a specific city
    char filterChoice;
    cout << "Would you like to filter the areas by city (y/n)? ";
    cin >> filterChoice;

    string filterCity;
    if (filterChoice == 'y' || filterChoice == 'Y') {
        cout << "Enter the city name to filter areas: ";
        cin.ignore();  // To clear the newline left by previous input
        getline(cin, filterCity);
    }

    // Display the headers
    cout << left << setw(30) << "Area Name" << setw(50) << "Address" << setw(30) << "City" << endl;
    cout << string(110, '-') << endl;

    // Loop through the areas and display them
    bool displayed = false;
    for (const auto& area : sortedAreas) {
        if (filterChoice == 'y' || filterChoice == 'Y') {
            // Only display areas in the specified city
            if (area.city == filterCity) {
                area.display();
                displayed = true;
            }
        }

        else
        {
            // Display all areas if no filter is applied
            area.display();
            displayed = true;
        }
    }

    if (!displayed)
    {
        cout << "No areas found matching the filter criteria.\n";
    }

    // Option to display details for a specific area
    char detailChoice;
    cout << "Would you like to view more details about a specific area (y/n)? ";
    cin >> detailChoice;

    if (detailChoice == 'y' || detailChoice == 'Y')
    {
        string areaName;
        cout << "Enter the area name to view details: ";
        cin.ignore();  // Clear the newline
        getline(cin, areaName);

        bool found = false;
        for (const auto& area : sortedAreas)
        {
            if (area.name == areaName)
            {
                found = true;
                cout << "\nDetails for Area: " << area.name << endl;
                cout << "Address: " << area.address << endl;
                cout << "City: " << area.city << endl;
                // You can add more detailed information here if needed, e.g., population, waste generation, etc.
                break;
            }
        }

        if (!found)
        {
            cout << "Area \"" << areaName << "\" not found.\n";
        }
    }

    // Ask the user if they want to display areas again
    char repeatChoice;
    cout << "Would you like to display the areas again (y/n)? ";
    cin >> repeatChoice;

    if (repeatChoice == 'y' || repeatChoice == 'Y') {
        displayAllAreas(); // Recursively call the function again to display the areas
    } else {
        cout << "Returning to main menu.\n";
    }
}
void AreaDatabase::updateAreaData()
{
    if (areas.empty())
    {
        cout << "No area data available to update. Please load data first.\n";
        return;
    }

    string searchName;
    cout << "\nEnter the area name to update: ";
    cin.ignore();
    getline(cin, searchName);

    // Ensure areas are sorted
    sortAreasByName();

    // Perform binary search
    auto it = lower_bound(areas.begin(), areas.end(), searchName, [](const Area& area, const string& name) {
        return area.name < name;
    });

    if (it == areas.end() || it->name != searchName)
    {
        cout << "Area \"" << searchName << "\" not found.\n";
        return;
    }

    // Display the current data
    Area* areaToUpdate = &(*it);
    cout << "\nCurrent data for \"" << searchName << "\":\n";
    cout << "Name: " << areaToUpdate->name << "\n";
    cout << "Address: " << areaToUpdate->address << "\n";
    cout << "City: " << areaToUpdate->city << "\n";

    // Confirm update
    char confirmUpdate;
    cout << "\nDo you want to update this area? (y/n): ";
    cin >> confirmUpdate;

    if (confirmUpdate != 'y' && confirmUpdate != 'Y')
    {
        cout << "Update cancelled.\n";
        return;
    }

    // Backup current data
    string oldName = areaToUpdate->name;
    string oldAddress = areaToUpdate->address;
    string oldCity = areaToUpdate->city;

    cin.ignore();
    cout << "Enter new area name (leave blank to keep unchanged): ";
    string newName;
    getline(cin, newName);
    if (!newName.empty())
    {
        areaToUpdate->name = newName;
    }

    cout << "Enter new address (leave blank to keep unchanged): ";
    string newAddress;
    getline(cin, newAddress);
    if (!newAddress.empty())
    {
        areaToUpdate->address = newAddress;
    }

    cout << "Enter new city (leave blank to keep unchanged): ";
    string newCity;
    getline(cin, newCity);
    if (!newCity.empty())
    {
        areaToUpdate->city = newCity;
    }

    cout << "Area data updated successfully.\n";

    // Log the changes
    cout << "\nUpdate Summary:\n";
    cout << "Previous Name: " << oldName << ", New Name: " << areaToUpdate->name << "\n";
    cout << "Previous Address: " << oldAddress << ", New Address: " << areaToUpdate->address << "\n";
    cout << "Previous City: " << oldCity << ", New City: " << areaToUpdate->city << "\n";

    // Undo option
    char undoChoice;
    cout << "\nWould you like to undo the update? (y/n): ";
    cin >> undoChoice;

    if (undoChoice == 'y' || undoChoice == 'Y')
    {
        areaToUpdate->name = oldName;
        areaToUpdate->address = oldAddress;
        areaToUpdate->city = oldCity;

        cout << "Update undone. Data restored to:\n";
        cout << "Name: " << areaToUpdate->name << "\n";
        cout << "Address: " << areaToUpdate->address << "\n";
        cout << "City: " << areaToUpdate->city << "\n";
    }

    // Ask to update another area
    char continueChoice;
    cout << "\nWould you like to update another area? (y/n): ";
    cin >> continueChoice;

    if (continueChoice == 'y' || continueChoice == 'Y')
    {
        updateAreaData(); // Recursive call
    }


    else
    {
        cout << "Returning to main menu...\n";
    }

    // Additional logs or user prompts can be added to expand the code further
    cout << "End of update process.\n";
}

void AreaDatabase::deleteAreaByName()
 {
    // Step 1: Check if there is any data to delete
    if (areas.empty())
    {
        cout << "No area data available to delete. Please load data first.\n";
        return;
    }

    // Step 2: Get the name of the area to delete
    string searchName;
    cout << "\nEnter the area name to delete: ";
    cin.ignore(); // Clear input buffer
    getline(cin, searchName);

    // Step 3: Search for the matching areas using linear search
    vector<int> matchingIndexes;
    for (size_t i = 0; i < areas.size(); i++)
    {
        if (areas[i].name == searchName)
        {
            matchingIndexes.push_back(i);
        }
    }

    // Step 4: Check if any areas were found
    if (matchingIndexes.empty())
    {
        cout << "No areas found with the name \"" << searchName << "\".\n";
        return;
    }

    // Step 5: Display all matching areas
    cout << "\nMatching areas found:\n";

    cout << left << setw(10) << "Index" << setw(30) << "Name" << setw(50) << "Address" << setw(30) << "City" << "\n";

    cout << string(110, '-') << "\n";

    for (size_t i = 0; i < matchingIndexes.size(); i++)
    {
        int index = matchingIndexes[i];
        const auto& area = areas[index];
        cout << left << setw(10) << i + 1 << setw(30) << area.name << setw(50) << area.address << setw(30) << area.city << "\n";
    }

    // Step 6: Select the specific area to delete
    int deleteIndex = 0;
    if (matchingIndexes.size() > 1)
    {
        cout << "\nMultiple matching areas found. Enter the index of the area you want to delete: ";
        cin >> deleteIndex;

        if (deleteIndex < 1 || deleteIndex > (int)matchingIndexes.size())
        {
            cout << "Invalid selection. Deletion process cancelled.\n";
            return;
        }
        deleteIndex = matchingIndexes[deleteIndex - 1];
    }

    else
    {
        deleteIndex = matchingIndexes[0];
    }

    // Step 7: Confirm the deletion
    const Area& areaToDelete = areas[deleteIndex];
    cout << "\nYou selected the following area to delete:\n";

    cout << "Name: " << areaToDelete.name << "\n";

    cout << "Address: " << areaToDelete.address << "\n";

    cout << "City: " << areaToDelete.city << "\n";

    char confirmDelete;
    cout << "\nAre you sure you want to delete this area? (y/n): ";

    cin >> confirmDelete;

    if (confirmDelete != 'y' && confirmDelete != 'Y')
    {
        cout << "Deletion process cancelled.\n";
        return;
    }

    // Step 8: Backup the data for undo
    Area backupArea = areaToDelete;

    // Step 9: Delete the area
    areas.erase(areas.begin() + deleteIndex);
    cout << "\nArea \"" << areaToDelete.name << "\" deleted successfully.\n";

    // Step 10: Offer undo option
    char undoChoice;
    cout << "\nWould you like to undo the deletion? (y/n): ";
    cin >> undoChoice;

    if (undoChoice == 'y' || undoChoice == 'Y')
    {
        areas.push_back(backupArea);
        cout << "\nDeletion undone. Area data restored.\n";
    }


    else
    {
        cout << "\nDeletion confirmed.\n";
    }

    // Step 11: Ask if the user wants to delete another area
    char continueChoice;
    cout << "\nWould you like to delete another area? (y/n): ";
    cin >> continueChoice;

    if (continueChoice == 'y' || continueChoice == 'Y')
    {
        deleteAreaByName(); // Recursive call for another deletion
    }


    else
    {
        cout << "\nReturning to the main menu...\n";
    }

    // Step 12: Additional log (Optional, extend as needed)
    cout << "Deletion process completed. Thank you.\n";

    // Note: The code can be further expanded by:
    // 1. Logging each action (e.g., log deleted areas to a file).
    // 2. Adding additional error checks and validations.
    // 3. Enhancing the user interface for more detailed prompts.
}


void AreaDatabase::addNewArea()
{
    Area newArea;
    string confirmation;
    bool isDuplicate = false;

    cout << "=== Add New Area ===\n";

    // Step 1: Input Area Name
    do
    {
        cout << "Enter area name: ";
        cin.ignore();
        getline(cin, newArea.name);

        // Validate non-empty input
        if (newArea.name.empty())
        {
            cout << "Error: Area name cannot be empty. Please try again.\n";
            continue;
        }

        // Check for duplicate names
        isDuplicate = false;
        for (const auto& area : areas)
        {
            if (area.name == newArea.name)
            {
                cout << "Error: An area with the name \"" << newArea.name << "\" already exists. Please enter a unique name.\n";
                isDuplicate = true;
                break;
            }
        }
    } while (newArea.name.empty() || isDuplicate);

    // Step 2: Input Address
    do {
        cout << "Enter address: ";
        getline(cin, newArea.address);

        // Validate non-empty input
        if (newArea.address.empty()) {
            cout << "Error: Address cannot be empty. Please try again.\n";
        }
    } while (newArea.address.empty());

    // Step 3: Input City
    do {
        cout << "Enter city: ";
        getline(cin, newArea.city);

        // Validate non-empty input
        if (newArea.city.empty()) {
            cout << "Error: City cannot be empty. Please try again.\n";
        }
    } while (newArea.city.empty());

    // Step 4: Confirm the Input
    cout << "\nPlease review the entered details:\n";
    cout << left << setw(30) << "Area Name" << setw(50) << "Address" << setw(30) << "City" << endl;
    cout << left << setw(30) << newArea.name << setw(50) << newArea.address << setw(30) << newArea.city << endl;

    do {
        cout << "Is the information correct? (y/n): ";
        cin >> confirmation;

        if (confirmation == "n" || confirmation == "N")
        {
            cout << "Operation cancelled. Returning to menu.\n";
            return;
        }

        else if (confirmation != "y" && confirmation != "Y")
        {
            cout << "Invalid input. Please enter 'y' or 'n'.\n";
        }
    } while (confirmation != "y" && confirmation != "Y");

    // Step 5: Add to the Area List
    areas.push_back(newArea);
    cout << "\nArea data added successfully.\n";

    // Step 6: Log the New Area Addition
    cout << "Logging the new area addition...\n";
    // Example log statement - Replace with actual file or database logging
    cout << "Log: New area \"" << newArea.name << "\" in \"" << newArea.city << "\" added.\n";

    // Step 7: Offer to Add Another Area
    char addAnother;
    cout << "\nWould you like to add another area? (y/n): ";
    cin >> addAnother;

    if (addAnother == 'y' || addAnother == 'Y')
    {
        addNewArea();  // Recursive call to add another area
    }

    else
    {
        cout << "Returning to the main menu...\n";
    }
}

    void AreaDatabase::saveToFile() const {
    if (areas.empty())
    {
        cout << "No area data to save. Please add data before saving.\n";
        return;
    }

    string filename;
    string confirmation;
    ofstream file;

    // Step 1: Input Filename
    do {
        cout << "Enter the filename to save area data to: ";
        cin >> filename;

        // Confirm overwriting existing file
        ifstream checkFile(filename);
        if (checkFile)
        {
            cout << "Warning: File \"" << filename << "\" already exists. Overwriting will replace its content.\n";
            cout << "Do you want to proceed? (y/n): ";
            cin >> confirmation;

            if (confirmation == "n" || confirmation == "N")
            {
                cout << "Operation cancelled. Returning to menu.\n";
                return;
            }

            else if (confirmation != "y" && confirmation != "Y")
            {
                cout << "Invalid input. Please enter 'y' or 'n'.\n";
                checkFile.close();
                continue;
            }
        }
        checkFile.close();

        // Attempt to open the file for writing
        file.open(filename, ios::out | ios::trunc);
        if (!file)
        {
            cout << "Error: Could not open file \"" << filename << "\" for writing. Please try again.\n";
        }
    } while (!file);

    // Step 2: Save Data to File
    cout << "Saving area data to \"" << filename << "\"...\n";
    for (const auto& area : areas)
    {
        file << area.name << "," << area.address << "," << area.city << endl;

        // Optional: Log each record saved
        cout << "Log: Saved area \"" << area.name << "\" to file.\n";
    }
    file.close();
    cout << "Area data successfully saved to file \"" << filename << "\".\n";

    // Step 3: Backup Option
    char createBackup;
    cout << "Would you like to create a backup of this file? (y/n): ";
    cin >> createBackup;

    if (createBackup == 'y' || createBackup == 'Y')
    {
        string backupFilename = filename + ".bak";
        ifstream sourceFile(filename, ios::binary);
        ofstream backupFile(backupFilename, ios::binary);

        if (sourceFile && backupFile)
        {
            backupFile << sourceFile.rdbuf();
            sourceFile.close();
            backupFile.close();
            cout << "Backup file \"" << backupFilename << "\" created successfully.\n";
        }

        else
        {
            cout << "Error: Unable to create backup file \"" << backupFilename << "\".\n";
        }
    }

}

    void AreaDatabase::enterPopulationData()
    {
    int count;
    cout << "Enter the number of areas for which you want to provide population data: ";

    // Input validation for count
    while (true) {
        cin >> count;
        if (count > 0) break;
        else {
            cout << "Invalid number. Please enter a positive integer: ";
        }
    }

    populationData.clear();

    for (int i = 0; i < count; ++i)
    {
        string areaName;
        double population;
        char editOption;

        cout << "\nEnter area name #" << (i + 1) << ": ";
        cin.ignore();
        getline(cin, areaName);

        // Remove any extra spaces from the area name
        areaName.erase(remove(areaName.begin(), areaName.end(), ' '), areaName.end());

        // Input validation for population
        while (true) {
            cout << "Enter population for " << areaName << ": ";
            cin >> population;
            if (population >= 0) break;

            else
            {
                cout << "Population cannot be negative. Please enter a valid population: ";
            }
        }

        // Check if the area name already exists in the population data
        auto it = populationData.find(areaName);

        if (it != populationData.end())
        {
            cout << "Population data already exists for \"" << areaName << "\". Do you want to update it? (y/n): ";
            cin >> editOption;

            // If user wants to update existing data
            if (editOption == 'y' || editOption == 'Y')
            {
                populationData[areaName] = population;
                cout << "Population data for \"" << areaName << "\" updated successfully.\n";
            }

             else
            {
                cout << "Population data for \"" << areaName << "\" not changed.\n";
            }
        }
         else
        {
            // Otherwise, add the new data
            populationData[areaName] = population;
            cout << "Population data for \"" << areaName << "\" added successfully.\n";
        }

        // Ask if user wants to add more population data
        char addMore;
        cout << "\nDo you want to add population data for another area? (y/n): ";
        cin >> addMore;

        if (addMore == 'n' || addMore == 'N')
        {
            break; // Exit the loop if user does not want to add more data
        }

        else if (addMore != 'y' && addMore != 'Y')
        {
            cout << "Invalid input. Please enter 'y' for yes or 'n' for no.\n";
            // Keep asking until valid input
            while (true) {
                cout << "\nDo you want to add population data for another area? (y/n): ";
                cin >> addMore;
                if (addMore == 'y' || addMore == 'Y') break;
                else if (addMore == 'n' || addMore == 'N') break;

                else
                {
                    cout << "Invalid input. Please enter 'y' for yes or 'n' for no.\n";
                }
            }
        }
    }

    // Ask the user if they want to save the population data to a file
    char saveOption;
    cout << "\nWould you like to save the population data to a file? (y/n): ";
    cin >> saveOption;

    if (saveOption == 'y' || saveOption == 'Y')
    {
        string filename;
        ofstream file;

        // Step 1: Get filename from user
        cout << "Enter filename to save population data: ";
        cin >> filename;

        file.open(filename, ios::out | ios::trunc);

        // Step 2: Check if file is opened successfully
        if (!file)
        {
            cout << "Error: Could not open file for saving population data. Please check the file path and try again.\n";
            return;
        }

        // Step 3: Save data to the file
        cout << "Saving population data to \"" << filename << "\"...\n";
        file << "Area Name, Population\n"; // Header

        for (const auto& entry : populationData)
        {
            file << entry.first << "," << entry.second << endl;
            cout << "Log: Saved population data for \"" << entry.first << "\" to file.\n";
        }

        file.close();
        cout << "Population data successfully saved to \"" << filename << "\".\n";
    }

    // Step 4: Provide options for editing or deleting entries
    char editDeleteOption;
    cout << "\nWould you like to edit or delete any population data? (e/d/n): ";
    cin >> editDeleteOption;

    if (editDeleteOption == 'e' || editDeleteOption == 'E')
    {
        string editAreaName;
        cout << "Enter the area name to edit: ";
        cin.ignore();
        getline(cin, editAreaName);

        auto it = populationData.find(editAreaName);
        if (it != populationData.end())
        {
            double newPopulation;
            cout << "Enter the new population for \"" << editAreaName << "\": ";
            cin >> newPopulation;
            it->second = newPopulation;
            cout << "Population for \"" << editAreaName << "\" updated successfully.\n";
        }

        else
        {
            cout << "Area \"" << editAreaName << "\" not found in the population data.\n";
        }

    }
     else if (editDeleteOption == 'd' || editDeleteOption == 'D') {
        string deleteAreaName;
        cout << "Enter the area name to delete: ";
        cin.ignore();
        getline(cin, deleteAreaName);

        auto it = populationData.find(deleteAreaName);
        if (it != populationData.end())
        {
            populationData.erase(it);
            cout << "Population data for \"" << deleteAreaName << "\" deleted successfully.\n";
        }

        else
        {
            cout << "Area \"" << deleteAreaName << "\" not found in the population data.\n";
        }
    }

     else if (editDeleteOption != 'n' && editDeleteOption != 'N')
    {
        cout << "Invalid option. Please enter 'e' for edit, 'd' for delete, or 'n' for none.\n";
    }

    cout << "\nPopulation data successfully entered and processed.\n";
}
void AreaDatabase::calculateWasteGeneration() const {
    // Average waste generation rate in kg per person per day
    double wastePerCapita = 0.74;

    // Input validation for custom waste per capita rate
    char customRateOption;
    cout << "Do you want to input a custom waste generation rate? (y/n): ";
    cin >> customRateOption;

    if (customRateOption == 'y' || customRateOption == 'Y')
    {
        cout << "Enter the waste generation rate in kg per person per day: ";
        cin >> wastePerCapita;

        // Validate the input rate
        while (wastePerCapita <= 0) {
            cout << "Invalid input. Waste generation rate must be greater than 0. Please try again: ";
            cin >> wastePerCapita;
        }
        cout << "Custom waste generation rate set to " << wastePerCapita << " kg/day.\n";
    }

    if (populationData.empty())
    {
        cout << "No population data available. Please load population data first.\n";
        return;
    }

    if (areas.empty())
    {
        cout << "No area data available. Please load area data first.\n";
        return;
    }

    // Displaying header for waste calculation
    cout << left << setw(30) << "Area Name"
         << setw(50) << "Address"
         << setw(30) << "City"
         << setw(20) << "Population"
         << setw(20) << "Waste (kg/day)"
         << setw(20) << "Waste (kg/year)"
         << setw(20) << "Notes" << endl;

    cout << string(170, '-') << endl;

    // Calculate waste for each area
    for (const auto& area : areas)
    {
        string areaName = area.name;
        areaName.erase(remove(areaName.begin(), areaName.end(), ' '), areaName.end());

        auto it = populationData.find(areaName);

        if (it != populationData.end())
        {
            double population = it->second;
            double totalWastePerDay = population * wastePerCapita;
            double totalWastePerYear = totalWastePerDay * 365;

            // Check if the waste generation rate is too high or too low
            string notes;

            if (totalWastePerDay < 100)
            {
                notes = "Unrealistic low waste generation.";
            }

            else if (totalWastePerDay > 10000)
            {
                notes = "Unrealistic high waste generation.";
            }

            else
            {
                notes = "Normal waste generation.";
            }

            // Output detailed information for the area
            cout << left << setw(30) << area.name
                 << setw(50) << area.address
                 << setw(30) << area.city
                 << setw(20) << population
                 << setw(20) << totalWastePerDay
                 << setw(20) << totalWastePerYear
                 << setw(20) << notes << endl;

        }

        else
        {
            // No population data found for the area
            cout << left << setw(30) << area.name
                 << setw(50) << area.address
                 << setw(30) << area.city
                 << setw(20) << "No data"
                 << setw(20) << "No data"
                 << setw(20) << "No data"
                 << setw(20) << "No population data" << endl;
        }
    }

    // Provide additional information about the calculation
    char furtherActionOption;
    cout << "\nWould you like to perform additional actions on the waste generation data? (y/n): ";
    cin >> furtherActionOption;

    if (furtherActionOption == 'y' || furtherActionOption == 'Y')
    {
        // Let the user choose an action: filter data, calculate summary, etc.
        char actionChoice;
        cout << "Choose an action:\n";
        cout << "1. View summary of total waste\n";
        cout << "2. View areas with excessive waste generation\n";
        cout << "3. Export data to file\n";
        cout << "Enter choice (1-3): ";
        cin >> actionChoice;

        if (actionChoice == '1')
        {
            // Calculate the total waste generation for all areas
            double totalWaste = 0.0;
            for (const auto& area : areas)
            {
                string areaName = area.name;
                areaName.erase(remove(areaName.begin(), areaName.end(), ' '), areaName.end());

                auto it = populationData.find(areaName);
                if (it != populationData.end())
                {
                    double population = it->second;
                    totalWaste += population * wastePerCapita;
                }
            }
            cout << "Total waste generation for all areas (kg/day): " << totalWaste << endl;
        }

         else if (actionChoice == '2')
        {
            // Filter areas with excessive waste generation (more than 1000 kg/day)
            cout << "\nAreas with excessive waste generation (more than 1000 kg/day):\n";
            for (const auto& area : areas)
            {
                string areaName = area.name;
                areaName.erase(remove(areaName.begin(), areaName.end(), ' '), areaName.end());

                auto it = populationData.find(areaName);
                if (it != populationData.end())
                {
                    double population = it->second;
                    double totalWastePerDay = population * wastePerCapita;
                    if (totalWastePerDay > 1000)
                    {
                        cout << area.name << " (" << totalWastePerDay << " kg/day)\n";
                    }
                }
            }
        }

        else if (actionChoice == '3')
        {
            // Export data to a file
            string filename;
            cout << "Enter filename to save the waste data: ";
            cin >> filename;
            ofstream file(filename);

            if (!file)
            {
                cout << "Error: Could not open file for writing. Please check the file path.\n";
                return;
            }

            file << "Area Name, Address, City, Population, Waste (kg/day), Waste (kg/year), Notes\n";
            for (const auto& area : areas)
            {
                string areaName = area.name;
                areaName.erase(remove(areaName.begin(), areaName.end(), ' '), areaName.end());

                auto it = populationData.find(areaName);

                if (it != populationData.end())
                {
                    double population = it->second;
                    double totalWastePerDay = population * wastePerCapita;
                    double totalWastePerYear = totalWastePerDay * 365;
                    string notes;

                    if (totalWastePerDay < 100)
                    {
                        notes = "Unrealistic low waste generation.";
                    }

                    else if (totalWastePerDay > 10000)
                    {
                        notes = "Unrealistic high waste generation.";
                    }

                    else
                    {
                        notes = "Normal waste generation.";
                    }

                    file << area.name << "," << area.address << "," << area.city << ","
                         << population << "," << totalWastePerDay << "," << totalWastePerYear << "," << notes << endl;
                }
            }

            file.close();
            cout << "Waste data successfully saved to \"" << filename << "\".\n";
        }
        else
        {
            cout << "Invalid choice. Returning to the main menu.\n";
        }
    }

    cout << "\nWaste generation calculations are complete.\n";
}
void AreaDatabase::sortAreasByName()
{
    if (areas.empty())
    {
        cout << "No areas to sort. Please load area data first.\n";
        return;
    }

    // User chooses the sorting order
    char orderChoice;
    cout << "Choose sorting order:\n";
    cout << "1. Ascending (A-Z)\n";
    cout << "2. Descending (Z-A)\n";
    cout << "Enter your choice (1/2): ";
    cin >> orderChoice;

    if (orderChoice != '1' && orderChoice != '2')
    {
        cout << "Invalid choice. Please enter 1 or 2.\n";
        return;
    }

    // Perform sorting based on the user's choice
    if (orderChoice == '1')
    {
        sort(areas.begin(), areas.end(), [](const Area& a, const Area& b) {
            return a.name < b.name;
        });
        cout << "Areas sorted in ascending order by name.\n";
    }

    else
    {
        sort(areas.begin(), areas.end(), [](const Area& a, const Area& b) {
            return a.name > b.name;
        });
        cout << "Areas sorted in descending order by name.\n";
    }

    // Display the sorted list
    cout << "\nSorted Areas:\n";
    cout << left << setw(30) << "Area Name" << setw(50) << "Address" << setw(30) << "City" << endl;
    cout << string(110, '-') << endl;
    for (const auto& area : areas)
    {
        cout << left << setw(30) << area.name << setw(50) << area.address << setw(30) << area.city << endl;
    }

    // Additional options after sorting
    char furtherActionOption;
    cout << "\nWould you like to perform further actions on the sorted list? (y/n): ";
    cin >> furtherActionOption;

    if (furtherActionOption == 'y' || furtherActionOption == 'Y')
    {
        char actionChoice;
        cout << "\nChoose an action:\n";
        cout << "1. Save the sorted list to a file\n";
        cout << "2. Filter areas by starting letter\n";
        cout << "3. Search for an area by name\n";
        cout << "Enter your choice (1/2/3): ";
        cin >> actionChoice;

        if (actionChoice == '1')
        {
            // Save the sorted list to a file
            string filename;
            cout << "Enter filename to save the sorted list: ";
            cin >> filename;
            ofstream file(filename);

            if (!file)
            {
                cout << "Error: Could not open file for writing. Please check the file path.\n";
                return;
            }

            file << left << setw(30) << "Area Name" << setw(50) << "Address" << setw(30) << "City" << endl;
            file << string(110, '-') << endl;

            for (const auto& area : areas)
            {
                file << left << setw(30) << area.name << setw(50) << area.address << setw(30) << area.city << endl;
            }

            file.close();
            cout << "Sorted list successfully saved to \"" << filename << "\".\n";

        }

        else if (actionChoice == '2')
        {
            // Filter areas by starting letter
            char filterLetter;
            cout << "Enter the starting letter to filter areas: ";
            cin >> filterLetter;

            cout << "\nFiltered Areas Starting with '" << filterLetter << "':\n";
            cout << left << setw(30) << "Area Name" << setw(50) << "Address" << setw(30) << "City" << endl;
            cout << string(110, '-') << endl;

            bool found = false;
            for (const auto& area : areas)
            {
                if (tolower(area.name[0]) == tolower(filterLetter))
                {
                    found = true;
                    cout << left << setw(30) << area.name << setw(50) << area.address << setw(30) << area.city << endl;
                }
            }

            if (!found)
            {
                cout << "No areas found starting with '" << filterLetter << "'.\n";
            }

        }

        else if (actionChoice == '3')
        {
            // Search for an area by name
            string searchName;
            cout << "Enter the area name to search for: ";
            cin.ignore();
            getline(cin, searchName);

            cout << "\nSearch Results:\n";
            cout << left << setw(30) << "Area Name" << setw(50) << "Address" << setw(30) << "City" << endl;
            cout << string(110, '-') << endl;

            bool found = false;
            for (const auto& area : areas)
            {
                if (area.name.find(searchName) != string::npos)
                {
                    found = true;
                    cout << left << setw(30) << area.name << setw(50) << area.address << setw(30) << area.city << endl;
                }
            }

            if (!found)
            {
                cout << "No areas found containing \"" << searchName << "\".\n";
            }

        }

         else
        {
            cout << "Invalid choice. Returning to the main menu.\n";
        }
    }

    cout << "\nSorting operation complete.\n";
}
void AreaDatabase::filterAreasByCity()
{
    if (areas.empty())
    {
        cout << "No area data available. Please load data first.\n";
        return;
    }

    string city;
    cout << "Enter the city to filter areas: ";
    cin.ignore();
    getline(cin, city);

    // Convert input city to lowercase for case-insensitive comparison
    transform(city.begin(), city.end(), city.begin(), ::tolower);

    cout << "\nAreas in \"" << city << "\":\n";
    cout << left << setw(30) << "Area Name" << setw(50) << "Address" << setw(30) << "City" << endl;
    cout << string(110, '-') << endl;

    bool found = false;
    for (const auto& area : areas)
    {
        string areaCity = area.city;
        transform(areaCity.begin(), areaCity.end(), areaCity.begin(), ::tolower);

        if (areaCity == city)
        {
            found = true;
            area.display();
        }
    }

    if (!found)
    {
        cout << "No areas found in \"" << city << "\".\n";
        return;
    }

    // Additional options after displaying the filtered list
    char furtherActionOption;
    cout << "\nWould you like to perform additional actions on the filtered areas? (y/n): ";
    cin >> furtherActionOption;

    if (furtherActionOption == 'y' || furtherActionOption == 'Y')
    {
        char actionChoice;
        cout << "\nChoose an action:\n";
        cout << "1. Save the filtered areas to a file\n";
        cout << "2. Display the total number of areas in the city\n";
        cout << "3. Search filtered areas by name\n";
        cout << "Enter your choice (1/2/3): ";
        cin >> actionChoice;

        if (actionChoice == '1')
        {
            // Save the filtered areas to a file
            string filename;
            cout << "Enter filename to save the filtered list: ";
            cin >> filename;

            ofstream file(filename);
            if (!file)
            {
                cout << "Error: Could not open file for writing. Please check the file path.\n";
                return;
            }

            file << left << setw(30) << "Area Name" << setw(50) << "Address" << setw(30) << "City" << endl;
            file << string(110, '-') << endl;

            for (const auto& area : areas)
            {
                string areaCity = area.city;
                transform(areaCity.begin(), areaCity.end(), areaCity.begin(), ::tolower);

                if (areaCity == city)
                {
                    file << left << setw(30) << area.name << setw(50) << area.address << setw(30) << area.city << endl;
                }
            }

            file.close();
            cout << "Filtered list successfully saved to \"" << filename << "\".\n";

        }

        else if (actionChoice == '2')
        {
            // Count the total number of areas in the city
            int count = 0;
            for (const auto& area : areas)
            {
                string areaCity = area.city;
                transform(areaCity.begin(), areaCity.end(), areaCity.begin(), ::tolower);

                if (areaCity == city)
                {
                    ++count;
                }
            }

            cout << "Total number of areas in \"" << city << "\": " << count << "\n";

        }

         else if (actionChoice == '3')
        {
            // Search filtered areas by name
            string searchName;
            cout << "Enter the area name to search within \"" << city << "\": ";
            cin.ignore();
            getline(cin, searchName);

            cout << "\nSearch Results:\n";
            cout << left << setw(30) << "Area Name" << setw(50) << "Address" << setw(30) << "City" << endl;
            cout << string(110, '-') << endl;

            bool foundInSearch = false;
            for (const auto& area : areas)
            {
                string areaCity = area.city;
                transform(areaCity.begin(), areaCity.end(), areaCity.begin(), ::tolower);

                if (areaCity == city && area.name.find(searchName) != string::npos)
                {
                    foundInSearch = true;
                    area.display();
                }
            }

            if (!foundInSearch)
            {
                cout << "No areas found with the name \"" << searchName << "\" in \"" << city << "\".\n";
            }

        }

        else
        {
            cout << "Invalid choice. Returning to the main menu.\n";
        }
    }

    cout << "\nFiltering operation complete.\n";
}
void AreaDatabase::exportToCSV() const {
    if (areas.empty())
    {
        cout << "No area data available to export. Please load data first.\n";
        return;
    }

    string filename;
    cout << "Enter filename for export (e.g., areas.csv): ";
    cin >> filename;

    ofstream file(filename);
    if (!file)
    {
        cout << "Error: Could not open file \"" << filename << "\" for export.\n";
        return;
    }

    // Ask the user if they want to export all columns or select specific ones
    char exportChoice;
    cout << "Do you want to export all columns? (y/n): ";
    cin >> exportChoice;

    bool exportAll = (exportChoice == 'y' || exportChoice == 'Y');
    vector<string> columnsToExport;

    if (!exportAll)
    {
        cout << "Select columns to export:\n";
        cout << "1. Area Name\n";
        cout << "2. Address\n";
        cout << "3. City\n";
        cout << "Enter column numbers separated by spaces (e.g., 1 2): ";

        cin.ignore();
        string columnInput;
        getline(cin, columnInput);

        istringstream iss(columnInput);
        int columnNum;
        while (iss >> columnNum) {
            if (columnNum == 1) columnsToExport.push_back("Area Name");
            else if (columnNum == 2) columnsToExport.push_back("Address");
            else if (columnNum == 3) columnsToExport.push_back("City");
            else {
                cout << "Invalid column number: " << columnNum << ". Skipping.\n";
            }
        }

        if (columnsToExport.empty())
        {
            cout << "No valid columns selected. Export cancelled.\n";
            return;
        }
    }

    else
    {
        columnsToExport = {"Area Name", "Address", "City"};
    }

    // Write the CSV header
    for (size_t i = 0; i < columnsToExport.size(); ++i)
    {
        file << columnsToExport[i];
        if (i < columnsToExport.size() - 1) file << ",";
    }
    file << "\n";

    // Ask the user if they want to filter data by city before exporting
    char filterChoice;
    cout << "Do you want to filter data by city before export? (y/n): ";
    cin >> filterChoice;

    string filterCity;
    bool applyFilter = (filterChoice == 'y' || filterChoice == 'Y');
    if (applyFilter)
    {
        cout << "Enter the city to filter by: ";
        cin.ignore();
        getline(cin, filterCity);
        transform(filterCity.begin(), filterCity.end(), filterCity.begin(), ::tolower);
    }

    // Write the data
    bool found = false;
    for (const auto& area : areas)
    {
        if (applyFilter)
        {
            string areaCity = area.city;
            transform(areaCity.begin(), areaCity.end(), areaCity.begin(), ::tolower);
            if (areaCity != filterCity) continue;
        }

        found = true;

        for (size_t i = 0; i < columnsToExport.size(); ++i)
        {
            if (columnsToExport[i] == "Area Name") file << area.name;
            else if (columnsToExport[i] == "Address") file << area.address;
            else if (columnsToExport[i] == "City") file << area.city;

            if (i < columnsToExport.size() - 1) file << ",";
        }
        file << "\n";
    }

    if (!found && applyFilter)
    {
        cout << "No data found for the specified city \"" << filterCity << "\".\n";
    }

    file.close();
    cout << "Data exported to \"" << filename << "\" successfully.\n";

    // Ask if the user wants to view the exported data
    char viewChoice;
    cout << "Do you want to view the exported file? (y/n): ";
    cin >> viewChoice;

    if (viewChoice == 'y' || viewChoice == 'Y')
    {
        ifstream viewFile(filename);
        if (viewFile)
        {
            cout << "\nExported Data:\n";
            string line;
            while (getline(viewFile, line)) {
                cout << line << "\n";
            }
            viewFile.close();
        }

        else
        {
            cout << "Error: Could not open file \"" << filename << "\" for viewing.\n";
        }
    }
}
void AreaDatabase::calculateAveragePopulation() const
 {
    if (populationData.empty())
    {
        cout << "No population data available. Please enter population data first.\n";
        return;
    }

    // Display options for filtering data before calculation
    char filterChoice;
    cout << "Do you want to calculate the average population for all areas or filter by city? (a: All, c: City): ";
    cin >> filterChoice;

    double totalPopulation = 0;
    int count = 0;

    if (filterChoice == 'c' || filterChoice == 'C')
    {
        // Filter by city
        string filterCity;
        cout << "Enter the city to calculate the average population for: ";
        cin.ignore();
        getline(cin, filterCity);
        transform(filterCity.begin(), filterCity.end(), filterCity.begin(), ::tolower);

        for (const auto& area : areas)
        {
            string areaCity = area.city;
            transform(areaCity.begin(), areaCity.end(), areaCity.begin(), ::tolower);

            if (areaCity == filterCity)
            {
                string areaName = area.name;
                areaName.erase(remove(areaName.begin(), areaName.end(), ' '), areaName.end());

                auto it = populationData.find(areaName);
                if (it != populationData.end())
                {
                    totalPopulation += it->second;
                    ++count;
                }
            }
        }

        if (count == 0)
        {
            cout << "No population data found for areas in the city \"" << filterCity << "\".\n";
            return;
        }

        cout << "Calculating average population for city \"" << filterCity << "\"...\n";
    }

    else
    {
        // Calculate for all areas
        cout << "Calculating average population for all areas...\n";
        for (const auto& entry : populationData)
        {
            totalPopulation += entry.second;
            ++count;
        }
    }

    // Calculate average population
    double averagePopulation = totalPopulation / count;
    cout << "Total Population: " << totalPopulation << endl;
    cout << "Number of Areas Considered: " << count << endl;
    cout << "The average population is: " << averagePopulation << endl;

    // Additional feature: Save results to a file
    char saveChoice;
    cout << "Do you want to save the results to a file? (y/n): ";
    cin >> saveChoice;

    if (saveChoice == 'y' || saveChoice == 'Y')
    {
        string filename;
        cout << "Enter the filename to save results (e.g., average_population.txt): ";
        cin >> filename;

        ofstream file(filename);
        if (!file)
        {
            cout << "Error: Could not open file \"" << filename << "\" for writing.\n";
            return;
        }
string filterCity;
        file << "Population Analysis Results\n";
        file << "----------------------------\n";
        file << "Total Population: " << totalPopulation << "\n";
        file << "Number of Areas Considered: " << count << "\n";
        file << "Average Population: " << averagePopulation << "\n";

        if (filterChoice == 'c' || filterChoice == 'C')
        {
            file << "Filtered City: " << filterCity << "\n";
        } else {
            file << "Filter: None (All Areas Considered)\n";
        }

        file.close();
        cout << "Results saved to \"" << filename << "\" successfully.\n";
    }

    // Additional feature: Display histogram of population distribution
    char histogramChoice;
    cout << "Do you want to view a population histogram? (y/n): ";
    cin >> histogramChoice;

    if (histogramChoice == 'y' || histogramChoice == 'Y') {
        cout << "\nPopulation Histogram:\n";
        cout << left << setw(20) << "Area Name" << setw(20) << "Population" << endl;
        cout << string(40, '-') << endl;

        for (const auto& entry : populationData) {
            cout << left << setw(20) << entry.first << setw(20) << entry.second << endl;
        }

        cout << "\nHistogram displayed successfully.\n";
    }
}
void AreaDatabase::printPopulationStats() const {
    if (populationData.empty())
    {
        cout << "No population data available.\n";
        return;
    }

    // Display population statistics
    cout << "Population Statistics:\n";
    cout << left << setw(30) << "Area Name" << setw(20) << "Population" << endl;
    cout << string(50, '-') << endl;

    for (const auto& entry : populationData) {
        cout << left << setw(30) << entry.first << setw(20) << entry.second << endl;
    }

    // Calculate total, minimum, maximum, and average population
    double totalPopulation = 0;
    double minPopulation = numeric_limits<double>::max();
    double maxPopulation = numeric_limits<double>::min();
    string minArea, maxArea;

    for (const auto& entry : populationData)
    {
        totalPopulation += entry.second;
        if (entry.second < minPopulation)
        {
            minPopulation = entry.second;
            minArea = entry.first;
        }

        if (entry.second > maxPopulation)
        {
            maxPopulation = entry.second;
            maxArea = entry.first;
        }
    }

    double averagePopulation = totalPopulation / populationData.size();

    cout << "\nSummary Statistics:\n";
    cout << "-------------------\n";
    cout << "Total Population: " << totalPopulation << endl;
    cout << "Average Population: " << averagePopulation << endl;
    cout << "Minimum Population: " << minPopulation << " (Area: " << minArea << ")\n";
    cout << "Maximum Population: " << maxPopulation << " (Area: " << maxArea << ")\n";

    // Option to filter data by a population range
    char filterChoice;
    cout << "\nDo you want to filter areas by population range? (y/n): ";
    cin >> filterChoice;

    if (filterChoice == 'y' || filterChoice == 'Y')
    {
        double lowerBound, upperBound;
        cout << "Enter the lower bound for population: ";
        cin >> lowerBound;
        cout << "Enter the upper bound for population: ";
        cin >> upperBound;

        cout << "\nAreas with population in the range [" << lowerBound << ", " << upperBound << "]:\n";
        cout << left << setw(30) << "Area Name" << setw(20) << "Population" << endl;
        cout << string(50, '-') << endl;

        for (const auto& entry : populationData)
        {
            if (entry.second >= lowerBound && entry.second <= upperBound)
            {
                cout << left << setw(30) << entry.first << setw(20) << entry.second << endl;
            }
        }
    }

    // Option to save statistics to a file
    char saveChoice;
    cout << "\nDo you want to save the statistics to a file? (y/n): ";
    cin >> saveChoice;

    if (saveChoice == 'y' || saveChoice == 'Y')
    {
        string filename;
        cout << "Enter the filename for saving statistics (e.g., population_stats.txt): ";
        cin >> filename;

        ofstream file(filename);
        if (!file)
        {
            cout << "Error: Could not open file \"" << filename << "\" for writing.\n";
            return;
        }

        file << "Population Statistics:\n";
        file << left << setw(30) << "Area Name" << setw(20) << "Population" << endl;
        file << string(50, '-') << endl;

        for (const auto& entry : populationData)
        {
            file << left << setw(30) << entry.first << setw(20) << entry.second << endl;
        }

        file << "\nSummary Statistics:\n";
        file << "-------------------\n";
        file << "Total Population: " << totalPopulation << endl;
        file << "Average Population: " << averagePopulation << endl;
        file << "Minimum Population: " << minPopulation << " (Area: " << minArea << ")\n";
        file << "Maximum Population: " << maxPopulation << " (Area: " << maxArea << ")\n";

        double lowerBound, upperBound;
        if (filterChoice == 'y' || filterChoice == 'Y')
        {
            file << "\nFiltered Areas (Range: [" << lowerBound << ", " << upperBound << "]):\n";
            file << left << setw(30) << "Area Name" << setw(20) << "Population" << endl;
            file << string(50, '-') << endl;

            for (const auto& entry : populationData)
            {
                if (entry.second >= lowerBound && entry.second <= upperBound)
                {
                    file << left << setw(30) << entry.first << setw(20) << entry.second << endl;
                }
            }
        }

        file.close();
        cout << "Statistics saved to \"" << filename << "\" successfully.\n";
    }

    // Option to visualize population distribution
    char visualizeChoice;
    cout << "\nDo you want to visualize the population distribution? (y/n): ";
    cin >> visualizeChoice;

    if (visualizeChoice == 'y' || visualizeChoice == 'Y')
    {
        cout << "\nPopulation Distribution:\n";
        for (const auto& entry : populationData)
        {
            cout << left << setw(30) << entry.first << " | ";
            int barLength = static_cast<int>(entry.second / maxPopulation * 50);
            cout << string(barLength, '*') << endl;
        }
    }
}

void AreaDatabase::calculateAverageWasteGeneration() const {
    double wastePerCapita = 0.74; // Average waste generation rate in kg per person per day

    if (populationData.empty())
    {
        cout << "No population data available. Please enter population data first.\n";
        return;
    }

    double totalWaste = 0;
    int areaCount = 0;

    cout << "\n--- Waste Generation Report ---\n";
    cout << left << setw(30) << "Area Name" << setw(20) << "Population" << setw(20) << "Waste (kg/day)" << endl;
    cout << string(70, '-') << endl;

    for (const auto& area : areas)
    {
        string areaName = area.name;
        areaName.erase(remove(areaName.begin(), areaName.end(), ' '), areaName.end());

        auto it = populationData.find(areaName);

        if (it != populationData.end())
        {
            double population = it->second;
            double areaWaste = population * wastePerCapita;

            totalWaste += areaWaste;
            ++areaCount;

            cout << left << setw(30) << area.name << setw(20) << population << setw(20) << areaWaste << endl;
        }

        else
        {
            cout << left << setw(30) << area.name << setw(20) << "No data" << setw(20) << "N/A" << endl;
        }
    }

    cout << string(70, '-') << endl;

    if (areaCount > 0)
    {
        double averageWaste = totalWaste / areaCount;
        cout << "\nThe average waste generation per area is: " << averageWaste << " kg/day.\n";
    }

    else
    {
        cout << "\nNo valid waste generation data available for the areas.\n";
    }
char DBL_MAX;
    // Additional insights
    double maxWaste = 0, minWaste = DBL_MAX;
    string maxArea, minArea;

    for (const auto& area : areas)
    {
        string areaName = area.name;
        areaName.erase(remove(areaName.begin(), areaName.end(), ' '), areaName.end());

        auto it = populationData.find(areaName);

        if (it != populationData.end())
        {
            double population = it->second;
            double areaWaste = population * wastePerCapita;

            if (areaWaste > maxWaste)
            {
                maxWaste = areaWaste;
                maxArea = area.name;
            }

            if (areaWaste < minWaste)
            {
                minWaste = areaWaste;
                minArea = area.name;
            }
        }
    }

    if (areaCount > 0)
    {
        cout << "Highest waste generation: " << maxArea << " with " << maxWaste << " kg/day.\n";
        cout << "Lowest waste generation: " << minArea << " with " << minWaste << " kg/day.\n";
    }

    // Optional: Save report
    string filename;
    cout << "\nWould you like to save the report to a file? (yes/no): ";
    string response;
    cin.ignore();
    getline(cin, response);

    if (response == "yes")
    {
        cout << "Enter the filename for the report: ";
        getline(cin, filename);

        ofstream reportFile(filename);
        if (!reportFile)
        {
            cout << "Error: Unable to create the file \"" << filename << "\".\n";
            return;
        }

        reportFile << "--- Waste Generation Report ---\n";
        reportFile << left << setw(30) << "Area Name" << setw(20) << "Population" << setw(20) << "Waste (kg/day)" << endl;
        reportFile << string(70, '-') << endl;

        for (const auto& area : areas) {
            string areaName = area.name;
            areaName.erase(remove(areaName.begin(), areaName.end(), ' '), areaName.end());

            auto it = populationData.find(areaName);

            if (it != populationData.end())
            {
                double population = it->second;
                double areaWaste = population * wastePerCapita;

                reportFile << left << setw(30) << area.name << setw(20) << population << setw(20) << areaWaste << endl;
            }

             else
            {
                reportFile << left << setw(30) << area.name << setw(20) << "No data" << setw(20) << "N/A" << endl;
            }
        }

        reportFile << string(70, '-') << endl;

        if (areaCount > 0)
        {
            reportFile << "\nThe average waste generation per area is: " << totalWaste / areaCount << " kg/day.\n";
            reportFile << "Highest waste generation: " << maxArea << " with " << maxWaste << " kg/day.\n";
            reportFile << "Lowest waste generation: " << minArea << " with " << minWaste << " kg/day.\n";
        }

        else
        {
            reportFile << "\nNo valid waste generation data available for the areas.\n";
        }

        reportFile.close();
        cout << "Report successfully saved to \"" << filename << "\".\n";
    }

    else
    {
        cout << "Report not saved.\n";
    }
}


int wastegeneration()
{
    AreaDatabase database;
    database.displayWelcomeMessage();

    int choice;

    do {
        cout << "\nMenu:\n";
        cout << "1. Load Area Data from File\n";
        cout << "2. Display All Areas\n";
        cout << "3. Update Area Data\n";
        cout << "4. Delete Area by Name\n";
        cout << "5. Add New Area\n";
        cout << "6. Save Area Data to File\n";
        cout << "7. Enter Population Data\n";
        cout << "8. Calculate Waste Generation\n";
        cout << "9. Sort Areas by Name\n";
        cout << "10. Filter Areas by City\n";
        cout << "11. Export Data to CSV\n";
        cout << "12. Calculate Average Population\n";
        cout << "13. Print Population Stats\n";
        cout << "14. Calculate Average Waste Generation\n"; // New option added
        cout << "15. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                database.loadFromFile();
                break;
            case 2:
                database.displayAllAreas();
                break;
            case 3:
                database.updateAreaData();
                break;
            case 4:
                database.deleteAreaByName();
                break;
            case 5:
                database.addNewArea();
                break;
            case 6:
                database.saveToFile();
                break;
            case 7:
                database.enterPopulationData();
                break;
            case 8:
                database.calculateWasteGeneration();
                break;
            case 9:
                database.sortAreasByName();
                break;
            case 10:
                database.filterAreasByCity();
                break;
            case 11:
                database.exportToCSV();
                break;
            case 12:
                database.calculateAveragePopulation();
                break;
            case 13:
                database.printPopulationStats();
                break;
            case 14:
                database.calculateAverageWasteGeneration(); // New case added
                break;
            case 15:
                cout << "Exiting program...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 15);

    return 0;
}



//module3
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <limits>
#include<iomanip>
#include <regex>
#include <algorithm>
#include <cctype>
#include <conio.h>
#include <windows.h>

// Define color codes using ANSI escape sequences
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define BOLD "\033[1m"


using namespace std;

// Structure to store waste data
struct Wastes
{
    // Type of waste (e.g., plastic, paper, etc.)
    std::string type;

    int quantity;

     // Location where the waste is stored
    std::string location;

     // Date when the waste was recorded
    std::string date;

     // Condition of the waste (e.g., clean, contaminated)
    std::string condition;
};

// Structure to store factory records
struct FactoryRecord
{
     // Type of waste sent to the factory

    std::string wasteType;
     // Total quantity sent

    int totalQuantity;
     // Factory name

    std::string factoryName;
     // Factory location

    std::string factoryLocation;
};

// Global containers to store records
std::vector<Wastes> wasteRecords;
std::vector<FactoryRecord> factoryRecords;

// Function prototypes
void loadCSV(const std::string& filePath);
void displayWasteRecords();
void displayMenu();
void searchWasteByType();
void sortWasteRecords();
void calculateStatistics();
void saveToCSV(const std::string& filePath);
void updateWasteRecord();
void deleteWasteRecord();
void addNewWasteRecord();
void sendWasteToFactory();
void viewFactoryRecords();
void validateInput(int& input, const std::string& prompt);
void validateInput(std::string& input, const std::string& prompt);
void displayFactoryRecords();
void displayFactoryRecordsByType(const std::string& wasteType);
void sortFactoryRecordsByQuantity();
void sortFactoryRecordsByName();
void sortFactoryRecordsByLocation();
void displayFactoryRecordsInTable();
void filterFactoryRecordsByCondition();
void exportFactoryRecordsToCSV();
void saveToCSV(const std::string& filePath);




// Function to load data from a CSV file
void loadCSV(const std::string& filePath)
 {
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file at " << filePath << std::endl;
        return;
    }

    std::string line;
    bool isHeader = true;

    // Reading the file line by line
    while (std::getline(file, line))
     {
        if (isHeader)
        {
            isHeader = false;  // Skip the header line
            continue;
        }

        // Split the line by commas and store data
        std::stringstream ss(line);
        std::string type, quantityStr, location, date, condition;

        if (std::getline(ss, type, ',') &&
            std::getline(ss, quantityStr, ',') &&
            std::getline(ss, location, ',') &&
            std::getline(ss, date, ',') &&
            std::getline(ss, condition, ','))
        {
            Wastes waste = {type, std::stoi(quantityStr), location, date, condition};
             // Add the waste record
            wasteRecords.push_back(waste);
        }
        else
        {
            std::cerr << "Malformed line in CSV file.\n";
        }
    }

    file.close();
    std::cout << "Data loaded successfully from the CSV file!\n";
}

// Function to display waste records
// Function to display waste records with options to filter, sort, and format outputt
// Function to display waste records in various formats
void displayWasteRecords()
{
    if (wasteRecords.empty())
    {
        std::cout << "No records to display!\n";
        return;
    }

    int choice;
    bool continueDisplaying = true;

    while (continueDisplaying)
    {
        std::cout << "\nChoose an option to display waste records:\n";
        std::cout << "1. View All Waste Records\n";
        std::cout << "2. View Waste Records by Condition\n";
        std::cout << "3. View Waste Records by Location\n";
        std::cout << "4. View Waste Records in Table Format\n";
        std::cout << "5. Back to Main Menu\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice)
         {
            case 1:
                // Display all records without formatting
                std::cout << "\nLoaded Waste Records:\n";
                std::cout << "--------------------------------------------\n";
                for (const auto& waste : wasteRecords)
                {
                    std::cout << "Type: " << waste.type
                              << ", Quantity: " << waste.quantity
                              << ", Location: " << waste.location
                              << ", Date: " << waste.date
                              << ", Condition: " << waste.condition << '\n';
                }
                std::cout << "--------------------------------------------\n";
                break;

            case 2:
            {
                // Filter records by condition
                std::string conditionFilter;
                std::cout << "Enter the condition to filter by (e.g., clean, contaminated): ";
                std::cin.ignore(); // Clear input buffer before reading string
                std::getline(std::cin, conditionFilter);

                bool found = false;
                std::cout << "\nFiltered Waste Records by Condition (" << conditionFilter << "):\n";
                std::cout << "--------------------------------------------\n";
                for (const auto& waste : wasteRecords)
                {
                    if (waste.condition == conditionFilter)
                    {
                        std::cout << "Type: " << waste.type
                                  << ", Quantity: " << waste.quantity
                                  << ", Location: " << waste.location
                                  << ", Date: " << waste.date
                                  << ", Condition: " << waste.condition << '\n';
                        found = true;
                    }
                }
                if (!found)
                {
                    std::cout << "No records found for condition: " << conditionFilter << '\n';
                }
                std::cout << "--------------------------------------------\n";
                break;
            }

            case 3:
                {
                // Filter records by location
                std::string locationFilter;
                std::cout << "Enter the location to filter by: ";
                std::cin.ignore(); // Clear input buffer before reading string
                std::getline(std::cin, locationFilter);

                bool found = false;
                std::cout << "\nFiltered Waste Records by Location (" << locationFilter << "):\n";
                std::cout << "--------------------------------------------\n";
                for (const auto& waste : wasteRecords)
                 {
                    if (waste.location == locationFilter)
                    {
                        std::cout << "Type: " << waste.type
                                  << ", Quantity: " << waste.quantity
                                  << ", Location: " << waste.location
                                  << ", Date: " << waste.date
                                  << ", Condition: " << waste.condition << '\n';
                        found = true;
                    }
                }
                if (!found)
                {
                    std::cout << "No records found for location: " << locationFilter << '\n';
                }
                std::cout << "--------------------------------------------\n";
                break;
            }

            case 4:
            {
                // View waste records in table format
                std::cout << "\nWaste Records in Table Format:\n";
                std::cout << std::setw(15) << std::left << "Type"
                          << std::setw(10) << "Quantity"
                          << std::setw(20) << "Location"
                          << std::setw(15) << "Date"
                          << std::setw(15) << "Condition" << '\n';
                std::cout << "--------------------------------------------------------\n";

                for (const auto& waste : wasteRecords)
                {
                    std::cout << std::setw(15) << std::left << waste.type
                              << std::setw(10) << waste.quantity
                              << std::setw(20) << waste.location
                              << std::setw(15) << waste.date
                              << std::setw(15) << waste.condition << '\n';
                }
                std::cout << "--------------------------------------------------------\n";
                break;
            }

            case 5:
                // Exit the display records loop
                continueDisplaying = false;
                break;

            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

//Using linear search algorithm
// Function to search for waste by type with enhancements
void searchWasteByType()
{
    if (wasteRecords.empty())
    {
        std::cout << "No waste records available for searching!\n";
        return;
    }

    std::string searchType;
    char searchOption;

    // Prompt user for the type of search
    std::cout << "Enter the type of waste to search for: ";
    std::cin.ignore();  // To ignore any extra newline characters
    std::getline(std::cin, searchType);

    // Ask the user if they want a case-insensitive search
    std::cout << "Would you like to perform a case-insensitive search? (y/n): ";
    std::cin >> searchOption;

    bool found = false;
    std::cout << "\nSearch Results:\n";
    std::cout << "--------------------------------------------\n";

    // Perform search with case-insensitive option if selected
    for (const auto& waste : wasteRecords)
    {
        bool match = false;

        // Case-insensitive comparison if selected by user
        if (searchOption == 'y' || searchOption == 'Y')
        {
            std::string wasteTypeLower = waste.type;
            std::transform(wasteTypeLower.begin(), wasteTypeLower.end(), wasteTypeLower.begin(), ::tolower);
            std::string searchTypeLower = searchType;
            std::transform(searchTypeLower.begin(), searchTypeLower.end(), searchTypeLower.begin(), ::tolower);

            match = (wasteTypeLower == searchTypeLower);
        }
         else
        {
            match = (waste.type == searchType);
        }

        // If there's a match, display the waste record
        if (match)
        {
            std::cout << "Type: " << waste.type
                      << ", Quantity: " << waste.quantity
                      << ", Location: " << waste.location
                      << ", Date: " << waste.date
                      << ", Condition: " << waste.condition << '\n';
            found = true;
        }
    }

    // If no records were found
    if (!found)
    {
        std::cout << "No records found for waste type: " << searchType << '\n';
    }

    std::cout << "--------------------------------------------\n";
}


// Function to sort waste records by quantity
void sortWasteRecords()
{
    if (wasteRecords.empty())
    {
        std::cout << "No waste records available for sorting!\n";
        return;
    }

    int choice;
    std::cout << "\nSelect the sorting order:\n";
    std::cout << "1. Sort by Quantity (Descending)\n";
    std::cout << "2. Sort by Quantity (Ascending)\n";
    std::cout << "3. Sort by Type (Alphabetical)\n";
    std::cout << "4. Sort by Location (Alphabetical)\n";
    std::cout << "Enter your choice (1-4): ";
    std::cin >> choice;

    switch (choice)
    {
        case 1:
            std::cout << "Sorting waste records by quantity (Descending)...\n";
            std::sort(wasteRecords.begin(), wasteRecords.end(), [](const Wastes& a, const Wastes& b) {
                return a.quantity > b.quantity;  // Sort in descending order
            });
            break;
        case 2:
            std::cout << "Sorting waste records by quantity (Ascending)...\n";
            std::sort(wasteRecords.begin(), wasteRecords.end(), [](const Wastes& a, const Wastes& b) {
                return a.quantity < b.quantity;  // Sort in ascending order
            });
            break;
        case 3:
            std::cout << "Sorting waste records by type (Alphabetical)...\n";
            std::sort(wasteRecords.begin(), wasteRecords.end(), [](const Wastes& a, const Wastes& b) {
                return a.type < b.type;  // Sort alphabetically by type
            });
            break;
        case 4:
            std::cout << "Sorting waste records by location (Alphabetical)...\n";
            std::sort(wasteRecords.begin(), wasteRecords.end(), [](const Wastes& a, const Wastes& b) {
                return a.location < b.location;  // Sort alphabetically by location
            });
            break;
        default:
            std::cout << "Invalid choice. No sorting applied.\n";
            return;
    }

    std::cout << "Records sorted successfully!\n";
    displayWasteRecords();  // Display the sorted records after sorting
}

// Function to evaluate composting feasibility for organic waste
void evaluateWasteCompostingFeasibility()
{
    int organicWaste = 0;

    for (const auto& waste : wasteRecords)
    {
        if (waste.type == "organic")
        {
            organicWaste += waste.quantity;
        }
    }

    if (organicWaste > 100)
    {
        std::cout << "Composting is feasible with " << organicWaste << " kg of organic waste.\n";
    }
    else
    {
        std::cout << "Insufficient organic waste for composting.\n";
    }
}

// Function to calculate statistics on the waste data
void calculateStatistics()
{
    if (wasteRecords.empty())
    {
        std::cout << "No records to calculate statistics!\n";
        return;
    }

    // Initializing variables to calculate statistics
    int totalQuantity = 0;
    int totalTypes = 0;
    int highestQuantity = std::numeric_limits<int>::min();
    int lowestQuantity = std::numeric_limits<int>::max();
    std::string highestType, lowestType;
    std::map<std::string, int> typeCounts;
    std::map<std::string, int> locationCounts;

    // Loop through the records and calculate statistics
    for (const auto& waste : wasteRecords)
    {
        totalQuantity += waste.quantity;
        typeCounts[waste.type] += waste.quantity;
        locationCounts[waste.location] += waste.quantity;

        if (waste.quantity > highestQuantity)
        {
            highestQuantity = waste.quantity;
            highestType = waste.type;
        }

        if (waste.quantity < lowestQuantity)
        {
            lowestQuantity = waste.quantity;
            lowestType = waste.type;
        }
    }

    // Calculating average quantity
    double averageQuantity = static_cast<double>(totalQuantity) / wasteRecords.size();

    std::cout << "\nWaste Statistics:\n";
    std::cout << "--------------------------------------------\n";
    std::cout << "Total Waste Quantity: " << totalQuantity << " kg\n";
    std::cout << "Average Waste Quantity: " << averageQuantity << " kg\n";

    // Displaying waste quantities by type
    std::cout << "Waste Quantities by Type:\n";
    for (const auto& typeCount : typeCounts)
    {
        std::cout << "Type: " << typeCount.first << ", Quantity: " << typeCount.second << " kg\n";
    }

    // Displaying waste quantities by location
    std::cout << "\nWaste Quantities by Location:\n";
    for (const auto& locationCount : locationCounts)
    {
        std::cout << "Location: " << locationCount.first << ", Quantity: " << locationCount.second << " kg\n";
    }

    // Displaying waste type with the highest quantity
    std::cout << "\nType with the Highest Quantity:\n";
    std::cout << "Type: " << highestType << ", Quantity: " << highestQuantity << " kg\n";

    // Displaying waste type with the lowest quantity
    std::cout << "\nType with the Lowest Quantity:\n";
    std::cout << "Type: " << lowestType << ", Quantity: " << lowestQuantity << " kg\n";

    std::cout << "--------------------------------------------\n";
}

// Function to implement waste trading between regions
void implementWasteTrading1()
 {
    std::map<std::string, int> regionWaste = {
        {"Region 1", 500},
        {"Region 2", 700},
        {"Region 3", 300}
    };

    std::cout << "Waste Trading Opportunities:\n";
    for (const auto& region : regionWaste)
    {
        std::cout << region.first << " has " << region.second << " kg of waste available for trading.\n";
    }
}


// Function to send waste to a factory

#include <algorithm>  // For std::transform
#include <cctype>     // For std::tolower, std::toupper

//Using linear search algorithm
// Function to send waste to a factory
void sendWasteToFactory()
{
    // Check if there are any waste records to send
    if (wasteRecords.empty())
    {
        std::cout << "No waste records available to send to the factory!\n";
        return;
    }

    // Get the type of waste the user wants to send (case insensitive)
    std::string wasteType;
    std::cout << "Enter the type of waste to send to the factory: ";
    std::cin.ignore();  // Clear any leftover newline character
    std::getline(std::cin, wasteType);

    // Convert the input to lowercase to make it case insensitive
    std::transform(wasteType.begin(), wasteType.end(), wasteType.begin(), [](unsigned char c){ return std::tolower(c); });

    // Calculate the total available quantity for the specified waste type
    int totalQuantity = 0;
    for (const auto& waste : wasteRecords)
    {
        // Convert waste type to lowercase for comparison
        std::string recordType = waste.type;
        std::transform(recordType.begin(), recordType.end(), recordType.begin(), [](unsigned char c){ return std::tolower(c); });

        if (recordType == wasteType)
        {
            totalQuantity += waste.quantity;
        }
    }

    // If no records of the specified waste type exist
    if (totalQuantity == 0)
    {
        std::cout << "No waste of type '" << wasteType << "' available for sending.\n";
        return;
    }

    // Display the total available quantity of the specified waste type
    std::cout << "Total available quantity of '" << wasteType << "': " << totalQuantity << " kg.\n";

    // Get the quantity to be sent to the factory
    int quantityToSend;
    do
    {
        std::cout << "Enter the quantity to send to the factory (between 1 and " << totalQuantity << "): ";
        std::cin >> quantityToSend;
        if (quantityToSend <= 0 || quantityToSend > totalQuantity)
        {
            std::cout << "Invalid quantity! Please try again.\n";
        }
    } while (quantityToSend <= 0 || quantityToSend > totalQuantity);

    // Get factory details
    std::string factoryName, factoryLocation;
    std::cin.ignore();  // Clear the input buffer
    std::cout << "Enter the factory name: ";
    std::getline(std::cin, factoryName);

    std::cout << "Enter the factory location: ";
    std::getline(std::cin, factoryLocation);

    // Deduct the quantity from waste records and manage remaining quantities
    int remainingToSend = quantityToSend;  // Track remaining quantity to be deducted
    for (auto& waste : wasteRecords)
    {
        std::string recordType = waste.type;
        std::transform(recordType.begin(), recordType.end(), recordType.begin(), [](unsigned char c){ return std::tolower(c); });

        if (recordType == wasteType && remainingToSend > 0)
        {
            // If the remaining quantity to send is greater than or equal to the waste quantity in this record
            if (remainingToSend >= waste.quantity)
            {
                remainingToSend -= waste.quantity;
                waste.quantity = 0;  // This waste record is exhausted
            }
            else
            {
                waste.quantity -= remainingToSend;
                remainingToSend = 0;  // All required quantity is deducted
            }
        }
    }

    // Add the transaction record to the factory records
    factoryRecords.push_back({wasteType, quantityToSend, factoryName, factoryLocation});

    // Confirmation message
    std::cout << "Successfully sent " << quantityToSend << " kg of '" << wasteType << "' to " << factoryName
              << " located at " << factoryLocation << ".\n";

    // Display remaining quantity of the specified waste type
    int remainingQuantity = 0;
    for (const auto& waste : wasteRecords)
    {
        std::string recordType = waste.type;
        std::transform(recordType.begin(), recordType.end(), recordType.begin(), [](unsigned char c){ return std::tolower(c); });

        if (recordType == wasteType)
        {
            remainingQuantity += waste.quantity;
        }
    }
    std::cout << "Remaining quantity of '" << wasteType << "': " << remainingQuantity << " kg.\n";

    // Ask if the user wants to view the updated waste records
    char viewChoice;
    std::cout << "Do you want to view the updated waste records? (y/n): ";
    std::cin >> viewChoice;

    if (viewChoice == 'y' || viewChoice == 'Y')
    {
        displayWasteRecords();  // Display the updated records
    }

    // Ask if the user wants to send more waste to the factory
    char continueChoice;
    std::cout << "Do you want to send more waste to a factory? (y/n): ";
    std::cin >> continueChoice;

    if (continueChoice == 'y' || continueChoice == 'Y')
    {
        sendWasteToFactory();  // Recursively call the function to send more waste
    }
    else
    {
        std::cout << "Returning to the main menu.\n";
    }
}





// Function to view factory records
#include <iomanip> // For formatting table display

// Function to display all factory records
void displayFactoryRecords()
{
    if (factoryRecords.empty())
    {
        std::cout << "No factory records to display!\n";
        return;
    }

    std::cout << "\nAll Factory Records:\n";
    std::cout << "--------------------------------------------\n";
    for (const auto& record : factoryRecords)
    {
        std::cout << "Type: " << record.wasteType
                  << ", Quantity: " << record.totalQuantity
                  << ", Factory: " << record.factoryName
                  << ", Location: " << record.factoryLocation << '\n';
    }
    std::cout << "--------------------------------------------\n";
}

// Function to display factory records by waste type
void displayFactoryRecordsByType(const std::string& wasteType)
 {
    bool found = false;

    std::cout << "\nFactory Records for Waste Type: " << wasteType << "\n";
    std::cout << "--------------------------------------------\n";
    for (const auto& record : factoryRecords) {
        if (record.wasteType == wasteType) {
            std::cout << "Type: " << record.wasteType
                      << ", Quantity: " << record.totalQuantity
                      << ", Factory: " << record.factoryName
                      << ", Location: " << record.factoryLocation << '\n';
            found = true;
        }
    }
    if (!found)
    {
        std::cout << "No records found for waste type: " << wasteType << '\n';
    }
    std::cout << "--------------------------------------------\n";
}

// Function to sort factory records by quantity
void sortFactoryRecordsByQuantity()
 {
    std::sort(factoryRecords.begin(), factoryRecords.end(), [](const FactoryRecord& a, const FactoryRecord& b) {
        return a.totalQuantity > b.totalQuantity;
    });
    std::cout << "Factory records sorted by quantity!\n";
}

// Function to sort factory records by factory name
void sortFactoryRecordsByName()
 {
    std::sort(factoryRecords.begin(), factoryRecords.end(), [](const FactoryRecord& a, const FactoryRecord& b) {
        return a.factoryName < b.factoryName;
    });
    std::cout << "Factory records sorted by factory name!\n";
}

// Function to sort factory records by location
void sortFactoryRecordsByLocation()
 {
    std::sort(factoryRecords.begin(), factoryRecords.end(), [](const FactoryRecord& a, const FactoryRecord& b) {
        return a.factoryLocation < b.factoryLocation;
    });
    std::cout << "Factory records sorted by location!\n";
}

// Function to display factory records in table format
void displayFactoryRecordsInTable()
{
    if (factoryRecords.empty())
    {
        std::cout << "No factory records to display!\n";
        return;
    }

    std::cout << "\nFactory Records (Table Format):\n";
    std::cout << std::left << std::setw(15) << "Waste Type"
              << std::setw(10) << "Quantity"
              << std::setw(20) << "Factory Name"
              << std::setw(15) << "Location" << '\n';
    std::cout << std::string(60, '-') << '\n';

    for (const auto& record : factoryRecords)
    {
        std::cout << std::left << std::setw(15) << record.wasteType
                  << std::setw(10) << record.totalQuantity
                  << std::setw(20) << record.factoryName
                  << std::setw(15) << record.factoryLocation << '\n';
    }
    std::cout << std::string(60, '-') << '\n';
}

// Function to export factory records to a CSV file
void exportFactoryRecordsToCSV()
 {
    std::string fileName;
    std::cout << "Enter the file name to save factory records (e.g., factory_records.csv): ";
    std::cin.ignore(); // Ignore leftover newline
    std::getline(std::cin, fileName);

    std::ofstream outFile(fileName);
    if (!outFile.is_open())
    {
        std::cout << "Failed to open file for writing. Please try again.\n";
        return;
    }

    // Write CSV headers
    outFile << "Waste Type,Quantity,Factory Name,Factory Location\n";

    // Write each factory record to the CSV file
    for (const auto& record : factoryRecords)
    {
        outFile << record.wasteType << ","
                << record.totalQuantity << ","
                << record.factoryName << ","
                << record.factoryLocation << "\n";
    }

    outFile.close();
    std::cout << "Factory records successfully exported to '" << fileName << "'.\n";
}
void saveToCSV(const std::string& filePath)
 {
    std::ofstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file for writing at " << filePath << '\n';
        return;
    }

    // Write the header for the CSV file
    file << "Type,Quantity,Location,Date,Condition\n";

    // Write waste data records to the file
    for (const auto& waste : wasteRecords)
    {
        file << waste.type << ',' << waste.quantity << ',' << waste.location
             << ',' << waste.date << ',' << waste.condition << '\n';
    }

    file.close();
    std::cout << "Data saved successfully to the CSV file!\n";
}

// Complete function to view factory records with menu options
void viewFactoryRecords()
{
    if (factoryRecords.empty())
    {
        std::cout << "No factory records to display!\n";
        return;
    }

    int choice;
    do
        {
        std::cout << "\n=== View Factory Records Menu ===\n";
        std::cout << "1. View All Factory Records\n";
        std::cout << "2. View Factory Records by Waste Type\n";
        std::cout << "3. Sort Factory Records by Quantity\n";
        std::cout << "4. Sort Factory Records by Factory Name\n";
        std::cout << "5. Sort Factory Records by Location\n";
        std::cout << "6. Display Factory Records in Table Format\n";
        std::cout << "7. Export Factory Records to CSV\n";
        std::cout << "8. Exit\n";
        std::cout << "Enter your choice (1-8): ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                displayFactoryRecords();
                break;

            case 2: {
                std::string wasteType;
                std::cout << "Enter the waste type to search for: ";
                std::cin.ignore(); // Ignore leftover newline
                std::getline(std::cin, wasteType);
                displayFactoryRecordsByType(wasteType);
                break;
            }

            case 3:
                sortFactoryRecordsByQuantity();
                break;

            case 4:
                sortFactoryRecordsByName();
                break;

            case 5:
                sortFactoryRecordsByLocation();
                break;

            case 6:
                displayFactoryRecordsInTable();
                break;

            case 7:
                exportFactoryRecordsToCSV();
                break;

            case 8:
                std::cout << "Exiting the view menu.\n";
                break;

            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 8);
}


// Function to update an existing waste record
void updateWasteRecord()
 {
    // Prompt user to enter the type of waste they want to update
    std::string searchType;
    std::cout << "Enter the type of waste you want to update: ";
    // To clear the input buffer
    std::cin.ignore();
    // Read the waste type to update
    std::getline(std::cin, searchType);

    // Flag to check if waste record is found
    bool found = false;

    // Loop through waste records to find the matching type
    for (auto& waste : wasteRecords)
     {
        if (waste.type == searchType)
        {
            found = true;  // Mark as found
            std::cout << "Updating record for waste type: " << waste.type << "\n";

            // Ask user to enter new details for the waste record
            std::cout << "Enter new quantity: ";
             // Update quantity
            std::cin >> waste.quantity;
            while (waste.quantity < 0)
            {
                // Validate positive quantity
                std::cout << "Quantity must be non-negative. Please enter a valid quantity: ";
                std::cin >> waste.quantity;
            }

            // Get new location from the user
            std::cout << "Enter new location: ";
            // Ignore any extra newline characters
            std::cin.ignore();
             // Read the new location
            std::getline(std::cin, waste.location);

            // Get new condition from the user
            std::cout << "Enter new condition (e.g., clean, contaminated): ";
            // Read the new condition
            std::getline(std::cin, waste.condition);

            // Display the updated record to the user
            std::cout << "Record updated successfully!\n";
            std::cout << "Updated waste record:\n";
            std::cout << "Type: " << waste.type
                      << ", Quantity: " << waste.quantity
                      << ", Location: " << waste.location
                      << ", Date: " << waste.date
                      << ", Condition: " << waste.condition << "\n";
                        // Exit loop after updating the first match
            break;
        }
    }

    // If no matching waste type was found
    if (!found)
    {
        std::cout << "No record found with waste type: " << searchType << "\n";
    }
}


// Function to calculate potential energy from waste
void calculateEnergyFromWaste1()
 {
    std::map<std::string, int> wasteEnergyPotential = {
        {"plastic", 1000},
        {"organic", 800},
        {"paper", 500},
        {"metal", 200}
    };

    int totalEnergy = 0;

    for (const auto& waste : wasteEnergyPotential)
    {
        totalEnergy += waste.second;
    }

    std::cout << "Total Potential Energy from Waste: " << totalEnergy << " MJ\n";
}


// Function to delete a waste record
void deleteWasteRecord()
{
    // Prompt the user to enter the type of waste to delete
    std::string searchType;
    std::cout << "Enter the type of waste you want to delete: ";
    // Clear any extra input buffer
    std::cin.ignore();
    // Read the waste type
    std::getline(std::cin, searchType);

    // Check if there are any records available before trying to delete
    if (wasteRecords.empty())
    {
        std::cout << "No records available to delete.\n";
        return;
    }

    // Search for the waste record using std::remove_if to find matching types
    auto it = std::remove_if(wasteRecords.begin(), wasteRecords.end(),
        [&searchType](const Wastes& waste) { return waste.type == searchType; });

    // If a matching waste type is found, erase the record
    if (it != wasteRecords.end())
    {
        // Erase the record from the vector using the iterator returned by std::remove_if
        wasteRecords.erase(it, wasteRecords.end());

        // Provide user feedback on successful deletion
        std::cout << "Record for waste type: " << searchType << " deleted successfully!\n";

        // Display the remaining waste records
        std::cout << "Updated waste records:\n";

        for (const auto& waste : wasteRecords)
        {
            std::cout << "Type: " << waste.type
                      << ", Quantity: " << waste.quantity
                      << ", Location: " << waste.location
                      << ", Date: " << waste.date
                      << ", Condition: " << waste.condition << "\n";
        }
    }

     else
    {
        // If no matching waste record was found, notify the user
        std::cout << "No record found with waste type: " << searchType << "\n";
    }

    // Optionally, you can implement a confirmation prompt to ask the user if they want to delete multiple records or go back
    char confirmDeletion;
    std::cout << "Do you want to delete another waste record? (Y/N): ";
    std::cin >> confirmDeletion;

    if (confirmDeletion == 'Y' || confirmDeletion == 'y')
    {
        // Recursive call to allow the user to delete more records if desired
        deleteWasteRecord();
    }

    else
    {
        // Optionally, return to the main menu or end the operation
        std::cout << "Returning to the main menu.\n";
    }
}

// Function to add a new waste record
// Function to add a new waste record
void addNewWasteRecord()
 {
    // Declare a new waste record object to be added
    Wastes newWaste;

    // Ask the user to enter waste type
    std::cout << "Enter waste type: ";
     // Clear any extra input buffer
    std::cin.ignore();
     // Read the type of waste
    std::getline(std::cin, newWaste.type);

    // Validate that waste type is not empty
    if (newWaste.type.empty())
    {
        std::cout << "Error: Waste type cannot be empty.\n";
        return;  // Exit if waste type is empty
    }

    // Ask the user to enter quantity
    std::cout << "Enter quantity (in kg): ";
    std::cin >> newWaste.quantity;

    // Validate that quantity is a positive number
    if (newWaste.quantity <= 0)
    {
        std::cout << "Error: Quantity must be greater than zero.\n";
        // Exit if the quantity is invalid
        return;
    }

    // Ask the user to enter location
    std::cout << "Enter location: ";

     // Clear the input buffer
    std::cin.ignore();
    // Read the location of the waste
    std::getline(std::cin, newWaste.location);

    // Validate that location is not empty

    if (newWaste.location.empty())
    {
        std::cout << "Error: Location cannot be empty.\n";
         // Exit if location is empty
        return;
    }

    // Ask the user to enter the date of the waste record
    std::cout << "Enter date (YYYY-MM-DD): ";
    std::getline(std::cin, newWaste.date);  // Read the date

    // Validate that the date is in correct format (YYYY-MM-DD)
    std::regex datePattern(R"(\d{4}-\d{2}-\d{2})");

    if (!std::regex_match(newWaste.date, datePattern))
    {
        std::cout << "Error: Date format must be YYYY-MM-DD.\n";
    // Exit if the date format is incorrect
        return;
    }

    // Ask the user to enter the condition of the waste
    std::cout << "Enter condition (e.g.,Recyclable,Non-Recyclable,Compostable): ";

    // Read the condition
    std::getline(std::cin, newWaste.condition);
    // Validate that condition is not empty
    if (newWaste.condition.empty())
    {
        std::cout << "Error: Condition cannot be empty.\n";
        // Exit if condition is empty
        return;
    }

    // Add the new waste record to the vector of records
    wasteRecords.push_back(newWaste);

    // Provide feedback to the user about the success of the operation
    std::cout << "New waste record added successfully!\n";

    // Optional: Ask the user if they want to add another record
    char confirmAdd;

    std::cout << "Do you want to add another waste record? (Y/N): ";
    std::cin >> confirmAdd;

    if (confirmAdd == 'Y' || confirmAdd == 'y')
    {
        // Recursively call the function to allow adding another record
        addNewWasteRecord();
    }

    else
    {
        // Optionally, return to the main menu or end the operation
        std::cout << "Returning to the main menu.\n";
    }

    // Optionally, print the updated list of waste records
    std::cout << "Updated waste records:\n";
    for (const auto& waste : wasteRecords)
    {
        std::cout << "Type: " << waste.type
                  << ", Quantity: " << waste.quantity
                  << ", Location: " << waste.location
                  << ", Date: " << waste.date
                  << ", Condition: " << waste.condition << "\n";
    }
}


// Function to validate integer input

void validateInput(int& input, const std::string& prompt)
 {
    std::cout << prompt;

    while (!(std::cin >> input))
    {
        // Clear error flag
        std::cin.clear();
        // Ignore incorrect input
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a valid integer: ";
    }
}


// Function to validate string input
void validateInput(std::string& input, const std::string& prompt)
{
    std::cout << prompt;
    std::getline(std::cin, input);

    while (input.empty())
    {
        std::cout << "Invalid input. Please enter a valid string: ";
        std::getline(std::cin, input);
    }
}

// Function to handle case-sensitive and partial matching of waste types
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

void selectFactoryForWasteType()
{
    // List of valid waste types
    vector<string> validWasteTypes = {"plastic", "E-waste", "organic", "paper", "glass", "metal"};

    string wasteType;
    cout << "Enter the type of waste to send to the factory: ";
    cin.ignore();  // Ignore extra newline characters
    getline(cin, wasteType);

    // Convert wasteType to lowercase for partial matching (if needed)
    string lowerWasteType = wasteType;
    transform(lowerWasteType.begin(), lowerWasteType.end(), lowerWasteType.begin(), ::tolower);

    // Find relevant waste types based on partial match
    vector<string> matchingTypes;
    for (const auto& validType : validWasteTypes)
    {
        // Check if the input wasteType partially matches any of the valid waste types
        if (validType.find(lowerWasteType) != string::npos)
        {
            matchingTypes.push_back(validType);
        }
    }

    if (matchingTypes.empty())
    {
        cout << "No matching waste types found for '" << wasteType << "'\n";
        return;
    }

    // Show matching waste types
    cout << "Matching waste types:\n";
    for (const auto& match : matchingTypes)
    {
        cout << match << "\n";
    }

    // Now you can recommend factories for the selected waste type
    cout << "Finding recommended factories for waste type: " << wasteType << "\n";

    // Mapping waste type to factories
    map<string, vector<string>> wasteFactories =
    {
        {"plastic", {"Plastic Renewal Corp, Shahapur", "Eco Green Plastics, Udyambag", "Recycle Plastic Industries, Nanawadi", "CleanTech Plastics, Angol", "Future Plastics Ltd, Tilakwadi"}},
        {"E-waste", {"Digital Waste Hub, Angol", "EcoTech Recycling, Nanawadi", "Green Electronics, Shahapur", "Smart E-Recycle Center, Udyambag", "Future Tech Renewals, Tilakwadi"}},
        {"glass", {"Green Glass Recycling, Shahapur","Belgaum Glassworks, Camp", "Shree Glass Recycling, Tilakwadi","Clear Glass Recyclers, Udyambag"}},
        {"metal", {"Belgaum Metal Works, Angol","MetalCraft Recycling, Tilakwadi","Fusion Metals, Udyambag","Belgaum Iron & Steel, Shahapur","Precision Metal Recyclers, Camp"}},
        {"organic", {"Belgaum Compost Facility, Sambra","Green Earth Organic Recycling, Tilakwadi","EcoCompost, Udyambag","Bio-Waste Solutions, Nanawadi","GreenTech Organics, Shahapur"}},
        {"paper", {"Belgaum Paper Mills, Tilakwadi","Green Paper Recycling, Udyambag","Eco Paper Recycling Ltd, Shahapur","Belgaum Waste Paper Recycling, Angol","PaperCycle, Nanawadi"}}
    };

    // Find the matching factories based on the first match
    for (const auto& matchingType : matchingTypes)
    {
        if (wasteFactories.find(matchingType) != wasteFactories.end())
        {
            cout << "Recommended factories for waste type '" << matchingType << "':\n";
            for (const auto& factory : wasteFactories[matchingType])
            {
                cout << factory << "\n";
            }
        }
    }
}

// Function to display the main menu in a retro game-like style
void displayMenu()
{
    int choice;
    do {
        // Print retro-style table with borders and menu options
        std::cout << "\n";
        std::cout << CYAN "+----------------------------------------+" RESET << "\n";
        std::cout << CYAN "|      " BOLD "WASTE MANAGEMENT SYSTEM MENU" RESET CYAN "      |" RESET << "\n";
        std::cout << CYAN "+----------------------------------------+" RESET << "\n";

        // Menu options with colorful text
        std::cout << YELLOW "| " RESET "1. " GREEN "View All Waste Records" RESET << "\n";
        std::cout << YELLOW "| " RESET "2. " GREEN "Search Waste by Type" RESET << "\n";
        std::cout << YELLOW "| " RESET "3. " GREEN "Sort Waste Records" RESET << "\n";
        std::cout << YELLOW "| " RESET "4. " GREEN "Calculate Waste Statistics" RESET << "\n";
        std::cout << YELLOW "| " RESET "5. " GREEN "Save Data to CSV" RESET << "\n";
        std::cout << YELLOW "| " RESET "6. " GREEN "Add New Waste Record" RESET << "\n";
        std::cout << YELLOW "| " RESET "7. " GREEN "Update Waste Record" RESET << "\n";
        std::cout << YELLOW "| " RESET "8. " GREEN "Delete Waste Record" RESET << "\n";
        std::cout << CYAN "+----------------------------------------+" RESET << "\n";
        std::cout << YELLOW "| " RESET "9. " GREEN "Select Waste Type for Factory" RESET << "\n"; // New case
        std::cout << YELLOW "| " RESET "10. " GREEN "Send Waste to Factory" RESET << "\n";
        std::cout << YELLOW "| " RESET "11. " GREEN "View Factory Records" RESET << "\n";
        std::cout << CYAN "+----------------------------------------+" RESET << "\n";
        std::cout << YELLOW "| " RESET "12. " GREEN "Exit" RESET << "\n";
        std::cout << CYAN "+----------------------------------------+" RESET << "\n";

        // Prompt for user input
        std::cout << BLUE "Enter your choice (1-12): " RESET;
        std::cin >> choice;

        // Menu logic
        switch (choice)
        {
            // Waste Records Management
            case 1:
                displayWasteRecords();
                break;
            case 2:
                searchWasteByType();
                break;
            case 3:
                sortWasteRecords();
                break;
            case 4:
                calculateStatistics();
                break;
            case 5:
                saveToCSV("updated_waste_data.csv");
                break;

            // Waste Record Modification
            case 6:
                addNewWasteRecord();
                break;
            case 7:
                updateWasteRecord();
                break;
            case 8:
                deleteWasteRecord();
                break;

            // Factory and Waste Processing
            case 9:
                // New case: Select waste type before sending to the factory
                selectFactoryForWasteType();
                break;
            case 10:
                sendWasteToFactory();
                break;
            case 11:
                viewFactoryRecords();
                break;

            // Exit the program
            case 12:
                std::cout << RED "Exiting... Goodbye!" RESET "\n";
                break;

            // Handle invalid choices
            default:
                std::cout << RED "Invalid choice. Please try again." RESET "\n";
                break;
        }
    } while (choice != 12);
}


// Main function to start the program

int wastetofactory()
{
    // Ensure the file path is correctly formatted for Windows
    std::string filePath = "C:\\MY CODES @KLES\\wastemagdata.csv";

    // Load existing data from the file
    loadCSV(filePath);

    // Display the menu for user interactions
    displayMenu();

    return 0;
}



// Function to display a welcome screen
void setColor(int colorCode)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorCode);
}

// Function to display a colorful and attractive welcome screen
void displayWelcomeScreen()
 {
    // Set color for the welcome message
    setColor(10); // Green text
    cout << "\n";
    cout << "*****************************************************\n";
    cout << "*                                                   *\n";
    cout << "*           Welcome to the Waste Management        *\n";
    cout << "*                  System!                         *\n";
    cout << "*                                                   *\n";
    cout << "*****************************************************\n";
    setColor(7); // Reset to default color
    cout << "\n";
    cout << "Please login to continue...\n\n";
}




// Function to securely get password input (Windows only)
string getPasswordInput()
 {
    string password = "";
    char ch;
    while (true) {
        ch = _getch();  // Get a character from input
        if (ch == 13)  // If Enter is pressed
            break;
        else if (ch == 8) {  // If Backspace is pressed
            if (password.length() > 0) {
                password.pop_back();  // Remove last character
                cout << "\b \b";  // Remove the last character from the screen
            }
        } else {
            password += ch;  // Add character to password
            cout << "*";  // Display asterisk for the entered character
        }
    }
    cout << "\n";
    return password;
}
void displayMainMenu()
{
    setColor(14);  // Yellow color for title
    cout << "==============================================\n";
    cout << "=          WASTE MANAGEMENT SYSTEM          =\n";
    cout << "==============================================\n";
    setColor(11);  // Cyan color for options
    cout << "  1. Waste Generation\n";
    setColor(13);  // Magenta color for options
    cout << "  2. Route Management\n";
    setColor(8);   // Blue color for options
      cout << "  3. Waste to Factory\n";
    setColor(9);   // Blue color for options
     cout << "  4. Selling recycled waste\n";
    setColor(10);
    cout << "  5. Exit\n";
    setColor(7);   // Reset to default color
    cout << "==============================================\n";
    cout << "Enter your choice: ";
}

// Main function
int main()
{
    // Display welcome screen
    displayWelcomeScreen();

    // Ask for login details
    string username;
    string password;

    // Ask for username
    setColor(11); // Cyan text
    cout << "Enter your username: ";
    setColor(7); // Reset to default
    cin >> username;

    // Ask for password after username input
    setColor(11); // Cyan text
    cout << "Enter your password: ";
    setColor(7); // Reset to default
    password = getPasswordInput();

    // Simulated authentication check
    if (username == "admin" && password == "1234")
    {
        setColor(10); // Green text
        cout << "\nLogin successful!\n";
    }
    else
    {
        setColor(12); // Red text
        cout << "\nLogin failed. Please try again.\n";
        setColor(7); // Reset to default
        return 0;  // Exit program if login fails
    }

    setColor(7); // Reset to default
    // Integrate modules here
    int choice = -1;
    do
    {
        displayMainMenu();
        cin >> choice;

        switch (choice)
        {
            case 1:
            {
                // Call the wastegeneration() function here
                cout << "Opening Waste Generation...\n";
                 wastegeneration();
                break;
            }

            case 2:
            {
                // Call the wastetofactory() function here
                cout << "Opening Route Management..\n";
                route();
                break;
            }

             case 3:
            {
                // Call the wastetofactory() function here
                cout << "Opening Waste to Factory...\n";
                wastetofactory();
                break;
            }

               case 4:
            {
                // Call the wastetofactory() function here
                cout << "Opening Recycled Products from waste...\n";
                 sellrecycled();
                break;
            }
            case 5:
            {
                setColor(12); // Red text
                cout << "Exiting the system. Goodbye!\n";
                setColor(7); // Reset to default
                break;
            }

            default:
            {
                setColor(12); // Red text
                cout << "Invalid choice. Please try again.\n";
                setColor(7); // Reset to default
                break;
            }
        }
    } while (choice != 5);

    return 0;
}




