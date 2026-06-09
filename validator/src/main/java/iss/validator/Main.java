package iss.validator;

import iss.validator.generated.Files;
import jakarta.xml.bind.JAXBContext;
import jakarta.xml.bind.Unmarshaller;
import jakarta.xml.bind.ValidationEvent;
import jakarta.xml.bind.ValidationEventHandler;
import javax.xml.XMLConstants;
import javax.xml.transform.stream.StreamSource;
import javax.xml.validation.Schema;
import javax.xml.validation.SchemaFactory;
import java.io.File;
import java.util.ArrayList;

public class Main {

    static class CollectingHandler implements ValidationEventHandler 
    {
        
        ArrayList<String> errors = new ArrayList<>();

        public boolean handleEvent(ValidationEvent event) {
            String level;
            if (event.getSeverity() == ValidationEvent.WARNING)
                level = "warning";
            else if (event.getSeverity() == ValidationEvent.ERROR)
                level = "error";
            else
                level = "fatal";

            String msg = event.getMessage();
            if (msg == null) msg = "";

            errors.add("{\"level\":\"" + level
                     + "\",\"message\":\"" + msg.replace("\"", "'") + "\"}");
            return true;
        }
    }

    public static void main(String[] args) throws Exception 
    {

        String xmlFile = args[0];
        String xsdFile = args[1];

        SchemaFactory sf = SchemaFactory.newInstance(XMLConstants.W3C_XML_SCHEMA_NS_URI);
        Schema schema = sf.newSchema(new File(xsdFile));

        JAXBContext jc = JAXBContext.newInstance(Files.class);
        Unmarshaller um = jc.createUnmarshaller();
        um.setSchema(schema);

        CollectingHandler handler = new CollectingHandler();
        um.setEventHandler(handler);
        um.unmarshal(new StreamSource(new File(xmlFile)));

        boolean valid = handler.errors.isEmpty();

        String errorsJson = "[";
        for (int i = 0; i < handler.errors.size(); i++) 
        {
            if (i > 0) errorsJson += ",";
            errorsJson += handler.errors.get(i);
        }
        errorsJson += "]";

        System.out.println("{\"valid\":" + valid + ",\"errors\":" + errorsJson + "}");
        System.exit(valid ? 0 : 1);
    }
}
