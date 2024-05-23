# Protege a ação com cookies
from django.views.decorators.csrf import csrf_protect


# controle/views.py
from django.shortcuts import render
from django.http import HttpResponse
import requests

NODEMCU_IP = "192.168.50.1"

@csrf_protect
def index(request):
    return render(request, 'controle/index.html')

@csrf_protect
def control_led(request):
    if request.method == "POST":
        action = request.POST.get("action")
        if action == "ligar":
            requests.get(f"http://{NODEMCU_IP}/led?action=ligar")
        elif action == "desligar":
            requests.get(f"http://{NODEMCU_IP}/led?action=desligar")
    return HttpResponse("ok")

