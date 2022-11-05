const text_esp = document.getElementById("text_esp"); 
const getButton = document.getElementById("getButton"); 
const form = document.getElementById("form");
const esp_resp = document.getElementById("esp_resp");
const name = document.getElementById("nombre_env");
const pass = document.getElementById("pass");

let number = 0;

getButton.addEventListener('click', async()=>{

    const resp =await fetch("https://192.168.1.96/info");
    const data = await resp.json();

    console.log(data);

    const status = data.status ? "verdadero": "false"

    text_esp.innerHTML  = `<h3>Respuesta del esp32 <small style="color:green;">Estado: ${status}</small></h3> 
                            <br> 
                            <p>El nombres es ${data.nombre}</p>
                            <p>La temperatura es ${data.temp}</p>
                            <p><strong>el numero es ${data.counter}</strong></p>
                            <br>
                            <i> VERSION : ${data.version}</i>
                            `

});


form.addEventListener('submit', async (e)=>{
    e.preventDefault()

    const user = {
        nombre: name.value,
        password: pass.value
    }

    str = JSON.stringify(user);

    console.log(str)
    const resp =await fetch("https://192.168.1.96/user",
                    {
                        method: "POST",
                        body : str,
                        headers: {"Content-type": "application/json; charset=UTF-8"}
                    });
    const data = await resp.json();

    esp_resp.innerText = `${data.status}`
    esp_resp.style.display = "block";
})