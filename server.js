const express = require('express');
const http = require('http');
const { Server } = require('socket.io');
const mysql = require('mysql2');

const app = express();
const server = http.createServer(app);
const io = new Server(server);

// Permite que o Express entenda os dados enviados pelo ESP32
app.use(express.urlencoded({ extended: true }));
app.use(express.json());

// Serve os arquivos HTML/CSS estáticos
app.use(express.static(__dirname));

// Configuração do banco de dados MySQL
const conexao = mysql.createConnection({
    host: 'localhost',
    user: 'root',
    password: '',
    database: 'bd_sensor'
});

conexao.connect(erro => {
    if (erro) throw erro;
    console.log('Conectado ao banco de dados MySQL.');
});

// NOVA ROTA: Escuta os dados enviados pelo Wi-Fi do Arduino/ESP
app.post('/sensor', (req, res) => {
    const status = req.body.status;
    console.log(`Status recebido via Wi-Fi: ${status}`);

    if (status) {
        // Envia para a página HTML em tempo real
        io.emit('atualizacao-sensor', status);

        // Salva no Banco de Dados
        const sql = 'INSERT INTO historico_proximidade (status_objeto) VALUES (?)';
        conexao.query(sql, [status], (erro) => {
            if (erro) console.error('Erro ao salvar no banco:', erro);
        });
        
        return res.sendStatus(200); // Responde OK para o ESP
    }
    
    res.sendStatus(400); // Responde Erro se não vier dados
});

server.listen(3000, '0.0.0.0', () => {
    console.log('Sistema de Wi-Fi rodando na porta 3000.');
});
