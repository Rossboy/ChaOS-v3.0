# ChaOS-v3.0
University Project


W plikach używamy:

extern PCB* ActiveProces; 

Jako procesu obecnie aktywnego, który posiada czas procesora.

Wszelkie błędy w obsłudze czego kolwiek załatwiamy w ten sposób:

ActiveProcess->error = 1;

O resztę się nie martwicie, załatwia to interpreter. Niezależnie czy chodzi o shella czy o obsługę procesu. 
