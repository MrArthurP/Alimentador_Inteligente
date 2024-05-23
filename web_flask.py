from flask import Flask, render_template, request
import requests


app = Flask(__name__)


NODEMCU_IP = "192.168.50.1" # Substitua pelo IP do seu NodeMCU

@app.route("/")
def index():
    # Alterar o caminho do template index.html se necessário
    return render_template("index.html")

@app.route("/led", methods=["POST"])
def control_led():
    action = request.form["action"]
    if action == "ligar":
        requests.get(f"http://{NODEMCU_IP}/led?action=ligar")
    elif action == "desligar":
        requests.get(f"http://{NODEMCU_IP}/led?action=desligar")

    return "ok"

if __name__ == "__main__":
    app.run(debug=True, host="0.0.0.0")
