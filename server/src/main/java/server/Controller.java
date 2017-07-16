package server;

import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.PrintWriter;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

@RestController
public class Controller {
    private PrintWriter log;

    public Controller() throws FileNotFoundException {
        log = new PrintWriter(new FileOutputStream("log.txt", true));
    }

    @RequestMapping("/")
    public String sensors() {
        return "<a href='/log?moisture=1&humidity=2&temperature=3'>Test</a>";
    }

    @RequestMapping("/log")
    public String sensors(Integer moisture, Integer humidity, Integer temperature) {
        String timestamp = LocalDateTime.now().format(DateTimeFormatter.ISO_LOCAL_DATE_TIME);
        String response = String.format("%s, %d, %d, %d", timestamp, moisture, humidity, temperature);

        System.out.println(response);
        log.println(response);
        log.flush();
        return "OK";
    }
}
