#include <FS.h>
#include <SPIFFS.h>
#include <mbedtls/x509_crt.h>

#define CA_CERT_PATH "/ca-cert.pem"
const char *client_cert_path = "/device-cert.pem";

void setup()
{
    Serial.begin(115200);

    // Initialize SPIFFS
    if (!SPIFFS.begin(true))
    {
        Serial.println("Failed to mount file system");
        return;
    }

    // Read CA certificate
    File file = SPIFFS.open(CA_CERT_PATH, "r");
    if (!file)
    {
        Serial.println("Failed to open CA certificate file");
        return;
    }

    String ca_cert;
    while (file.available())
    {
        ca_cert += (char)file.read();
    }
    file.close();
    ca_cert += '\0';  // Ensure null-termination

    // Read device certificate
    file = SPIFFS.open(client_cert_path, "r");
    if (!file)
    {
        Serial.println("Failed to open device certificate file");
        return;
    }

    String device_cert;
    while (file.available())
    {
        device_cert += (char)file.read();
    }
    file.close();
    device_cert += '\0';  // Ensure null-termination

    Serial.println("\nca_cert");
    Serial.println(ca_cert);
    Serial.println("\ndevice_cert");
    Serial.println(device_cert);

    // Initialize mbedTLS
    mbedtls_x509_crt ca;
    mbedtls_x509_crt device;
    mbedtls_x509_crt_init(&ca);
    mbedtls_x509_crt_init(&device);

    // Parse CA certificate
    int ret = mbedtls_x509_crt_parse(&ca, (const unsigned char *)ca_cert.c_str(), ca_cert.length());
    if (ret != 0)
    {
        Serial.printf("Failed to parse CA certificate. Error code: %d\n", ret);
    }
    else
    {
        Serial.println("CA certificate parsed successfully");
    }

    // Parse device certificate
    ret = mbedtls_x509_crt_parse(&device, (const unsigned char *)device_cert.c_str(), device_cert.length());
    if (ret != 0)
    {
        Serial.printf("Failed to parse device certificate. Error code: %d\n", ret);
    }
    else
    {
        Serial.println("Device certificate parsed successfully");
    }

    uint32_t flags;
    ret = mbedtls_x509_crt_verify(&device, &ca, NULL, NULL, &flags, NULL, NULL);
    if (ret != 0)
    {
        Serial.print("Client certificate verification failed. ");
        Serial.printf("Verification flags: %u\n", flags);
    }
    else
    {
        Serial.println("\n===================================");
        Serial.println("Certificates verified successfully");
        Serial.println("===================================\n");
    }

    // Free resources
    mbedtls_x509_crt_free(&ca);
    mbedtls_x509_crt_free(&device);
}

void loop()
{
    // Your main code
}
