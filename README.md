# Laboratorio: Control de la cámara en Unreal Engine

**Autor:** Raimon Rodríguez Allés  
**Repositorio:** [Actividad3Motores2](https://github.com/raimonunir/Actividad3Motores2)

---

## 1. Descripción del proyecto

Este proyecto tiene como objetivo experimentar con la cámara en Unreal Engine, modificando un proyecto de tipo **Third Person** en C++ para replicar un comportamiento similar a la cámara de un proyecto **Top Down**.  

Se exploran aspectos fundamentales de Unreal Engine como:

- Configuración de cámara desde C++  
- Prioridad de valores entre C++ y Blueprints  
- Movimiento del personaje relativo a la cámara o al mundo

El proyecto incluye además la funcionalidad de **alternar la cámara** entre Top Down y Third Person mediante la tecla `C`.

---

## 2. Objetivos de la práctica

- Mantener la cámara con orientación constante, sin rotar según el movimiento del personaje  
- Seguir al jugador, manteniéndolo centrado  
- Ubicar la cámara a cierta altura para ver el entorno inmediato  
- Configurar la cámara y movimiento desde C++, mostrando la interacción con Blueprints

---

## 3. Características principales

### 3.1 Componentes de cámara

- `USpringArmComponent` (CameraBoom): mantiene la cámara a distancia del personaje y gestiona su posición relativa  
- `UCameraComponent` (FollowCamera): representa la cámara propiamente dicha

### 3.2 Funcionalidad Toggle Camera

Se implementó la función `ToggleCameraMode()` para alternar entre Top Down y Third Person:

- **Top Down:** cámara fija, movimiento relativo al mundo  
- **Third Person:** cámara detrás del personaje, movimiento relativo a la cámara  
- **Rotación sincronizada:** al cambiar a Third Person, el personaje se alinea con la cámara para evitar movimientos descoordinados

```cpp
void AActividad3Character::ToggleCameraMode()
{
    if (bIsTopDownCamera)
    {
        CameraBoom->TargetArmLength = ThirdPersonArmLength;
        CameraBoom->SetRelativeRotation(ThirdPersonRotation);
        CameraBoom->bUsePawnControlRotation = true;
        CameraBoom->bInheritPitch = true;
        CameraBoom->bInheritRoll = true;
        CameraBoom->bInheritYaw = true;
        bUseCameraRelativeMovement = true;

        FRotator TargetRotation = CameraBoom->GetComponentRotation();
        TargetRotation.Pitch = 0.f;
        TargetRotation.Roll = 0.f;

        SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 10.f));
    }
    else
    {
        CameraBoom->TargetArmLength = TopDownArmLength;
        CameraBoom->SetRelativeRotation(TopDownRotation);
        CameraBoom->bUsePawnControlRotation = false;
        CameraBoom->bInheritPitch = false;
        CameraBoom->bInheritRoll = false;
        CameraBoom->bInheritYaw = false;
        bUseCameraRelativeMovement = false;
    }

    bIsTopDownCamera = !bIsTopDownCamera; 
}
```

## 4. Problema detectado

Al modificar el código C++, los cambios **no se reflejaban en el Blueprint `BP_ThirdPersonCharacter`**, incluso usando `Reset to Default`. Esto se debe a que:

- Unreal Engine copia los valores por defecto de C++ al crear un Blueprint
- Posteriormente, los valores del Blueprint tienen prioridad y sobrescriben los definidos en C++

---

## 5. Solución aplicada

1. Abrir el Blueprint del personaje
2. **Reparentar** el Blueprint a la clase C++ actualizada
3. Restablecer manualmente los valores de cámara y movimiento usando `Reset to Default` si fuera necesario
4. Guardar y ejecutar

Con esto, la cámara y el movimiento reflejan correctamente los valores de C++ y la alternancia Top Down ↔ Third Person funciona de manera coherente.

---

## 6. Consideraciones sobre UPROPERTY

- Variables definidas con `UPROPERTY` permiten modificar valores desde Blueprints
- Sin embargo, no eliminan el conflicto de prioridad con valores ya asignados en el Blueprint
- La solución definitiva fue **reparentar el Blueprint** para que tome los valores del constructor de C++

---

## 7. Uso del proyecto

1. Clonar el repositorio:

```bash
git clone https://github.com/raimonunir/Actividad3Motores2.git
```
2. Abrir el proyecto en Unreal Engine 5.4

3. Compilar el proyecto desde Visual Studio

4. Ejecutar el juego

5. Pulsar la tecla `C` para alternar entre cámara Top Down y Third Person

6. Controlar el personaje con el teclado o gamepad según la configuración de Enhanced Input

---

## 8. Resultados esperados

- La cámara mantiene orientación constante en Top Down
- La cámara sigue al personaje y mantiene la visibilidad del entorno
- En Third Person, el personaje siempre se mueve en la dirección de la cámara
- Alternar la cámara no provoca desalineación en el movimiento

---

## 9. Conclusiones

- La práctica permitió comprender la interacción entre C++ y Blueprints
- Se experimentó con la configuración de cámara y movimiento de personajes
- Se identificó y resolvió un problema habitual de prioridad de valores en Unreal Engine
- El proyecto sirve como base para futuros experimentos de cámara y control de personajes
