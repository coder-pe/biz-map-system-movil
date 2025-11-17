package com.aisoldev.bizmap

import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import org.json.JSONArray

class MainActivity : AppCompatActivity() {
    private lateinit var bizMapClient: BizMapClient
    private lateinit var statusText: TextView
    private lateinit var usernameInput: EditText
    private lateinit var passwordInput: EditText
    private lateinit var loginButton: Button
    private lateinit var searchButton: Button

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Inicializar vistas
        statusText = findViewById(R.id.statusText)
        usernameInput = findViewById(R.id.usernameInput)
        passwordInput = findViewById(R.id.passwordInput)
        loginButton = findViewById(R.id.loginButton)
        searchButton = findViewById(R.id.searchButton)

        // Crear cliente BizMap
        try {
            // CAMBIAR esta IP por la de tu backend
            bizMapClient = BizMapClient("http://10.0.2.2:8080") // 10.0.2.2 = localhost en emulador
            updateStatus("✓ Cliente BizMap inicializado")
        } catch (e: Exception) {
            updateStatus("✗ Error: ${e.message}")
            Log.e(TAG, "Error inicializando cliente", e)
            return
        }

        // Configurar botones
        loginButton.setOnClickListener {
            val username = usernameInput.text.toString()
            val password = passwordInput.text.toString()

            if (username.isEmpty() || password.isEmpty()) {
                updateStatus("Por favor ingresa usuario y contraseña")
                return@setOnClickListener
            }

            performLogin(username, password)
        }

        searchButton.setOnClickListener {
            performSearch()
        }

        searchButton.isEnabled = false
    }

    private fun performLogin(username: String, password: String) {
        updateStatus("Iniciando sesión...")
        loginButton.isEnabled = false

        bizMapClient.login(
            username = username,
            password = password,
            onSuccess = { accessToken, refreshToken, expiresIn ->
                runOnUiThread {
                    updateStatus("✓ Login exitoso!\nToken expira en $expiresIn segundos")
                    bizMapClient.setAuthToken(accessToken)
                    searchButton.isEnabled = true
                    loginButton.isEnabled = true
                    Log.i(TAG, "Access Token: ${accessToken.take(20)}...")
                }
            },
            onError = { statusCode, message ->
                runOnUiThread {
                    updateStatus("✗ Error en login:\nCódigo: $statusCode\n$message")
                    loginButton.isEnabled = true
                    Log.e(TAG, "Error login: $statusCode - $message")
                }
            }
        )
    }

    private fun performSearch() {
        updateStatus("Buscando productos...")
        searchButton.isEnabled = false

        // Coordenadas de ejemplo (Lima, Perú)
        val latitude = -12.0464
        val longitude = -77.0428

        bizMapClient.searchProducts(
            query = "laptop",
            latitude = latitude,
            longitude = longitude,
            radiusMeters = 5000,
            minPrice = 500.0,
            maxPrice = 2000.0,
            category = "Electrónica",
            limit = 10,
            onSuccess = { productsJson ->
                runOnUiThread {
                    try {
                        val products = JSONArray(productsJson)
                        val count = products.length()

                        val resultText = buildString {
                            append("✓ Encontrados $count productos:\n\n")
                            for (i in 0 until count.coerceAtMost(5)) {
                                val product = products.getJSONObject(i)
                                val name = product.optString("name", "N/A")
                                val price = product.optDouble("price", 0.0)
                                val distance = product.optDouble("distance_meters", 0.0)

                                append("${i + 1}. $name\n")
                                append("   Precio: S/. ${"%.2f".format(price)}\n")
                                append("   Distancia: ${distance.toInt()}m\n\n")
                            }
                            if (count > 5) {
                                append("... y ${count - 5} más")
                            }
                        }

                        updateStatus(resultText)
                        Log.i(TAG, "Productos encontrados: $count")
                    } catch (e: Exception) {
                        updateStatus("✗ Error parseando respuesta: ${e.message}")
                        Log.e(TAG, "Error parseando JSON", e)
                    }
                    searchButton.isEnabled = true
                }
            },
            onError = { statusCode, message ->
                runOnUiThread {
                    updateStatus("✗ Error en búsqueda:\nCódigo: $statusCode\n$message")
                    searchButton.isEnabled = true
                    Log.e(TAG, "Error búsqueda: $statusCode - $message")
                }
            }
        )
    }

    private fun updateStatus(text: String) {
        statusText.text = text
        Log.d(TAG, "Status: $text")
    }

    override fun onDestroy() {
        super.onDestroy()
        bizMapClient.destroy()
    }

    companion object {
        private const val TAG = "BizMapMain"
    }
}
