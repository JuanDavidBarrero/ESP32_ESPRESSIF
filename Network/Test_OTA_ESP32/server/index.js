import express from 'express';
import fs from 'fs';
import cors from 'cors'

const app = express();

app.use(cors());

app.get('/', (req, res) => {
    return res.json({
        msg:"Hola mundo"
    })
});


app.get("/firmware", (req, res) => {
    const filePath = `./firmware/firmware.bin`;

    fs.readFile(filePath, (err, file) => {
        if (err) {
            console.log(err);
            return res.status(500).send('Could not download file');
        }

        res.setHeader('Content-Type', 'application/x-binary');

        res.send(file)


    });
});

app.get('/update', (req, res) => {

    const data = {
        version: 0.1,
        file: "http://192.168.0.106:3000/firmware"
    }

    res.json(data);
});

const PORT = process.env.PORT || 3000;

app.listen(PORT, () => {
    console.log(`app is listening on port ${PORT}`);
});