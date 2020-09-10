#include "Server.h"

AsyncWebServer server(HTTP_PORT);
AsyncWebHandler *spiffsEditorHandler;

void initWebServer()
{
    server.serveStatic("/bootstrap.min.css", SPIFFS, "/bootstrap.min.css").setCacheControl("public, max-age=31536000");
    server.serveStatic("/favicon.png", SPIFFS, "/favicon.png").setCacheControl("public, max-age=31536000");
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm");

    server.addHandler(new SPIFFSEditor(SPIFFS, SPIFFS_EDITOR_LOGIN, SPIFFS_EDITOR_PASS));

    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404);
    });

    server.on(
        "/update",
        HTTP_POST,
        [](AsyncWebServerRequest *request) {
            shouldReboot = !Update.hasError();
            AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", shouldReboot ? "OK" : "FAIL");
            response->addHeader("Connection", "close");
            request->send(response);
        },
        [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
            if (!index)
            {
                Serial.println("Update Start");
                if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000))
                {
                    Update.printError(Serial);
                }
            }

            if (!Update.hasError())
            {
                if (Update.write(data, len) != len)
                {
                    Update.printError(Serial);
                }
            }

            if (final)
            {
                if (Update.end(true))
                {
                    Serial.println("Update Success");
                    request->redirect("/");
                }
                else
                {
                    Update.printError(Serial);
                }
            }
        });

    server.on("/wifi-conf", HTTP_POST, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "OK");
        response->addHeader("Connection", "close");
        request->send(response);

        WiFi.begin(request->getParam("ssid", true)->value().c_str(), request->getParam("pass", true)->value().c_str());
        WiFi.reconnect();
    });

    server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "OK");
        response->addHeader("Connection", "close");
        request->send(response);
        shouldReboot = true;
    });

    server.begin();
}
