#include "tienda.h"
#include "ui_tienda.h"
#include "acerca.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QMessageBox>
#include <QDebug>

Tienda::Tienda(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Tienda)
{
    ui->setupUi(this);

    m_subtotal = 0;
    cargarDatos();
    inicializarWidgets();
}

Tienda::~Tienda()
{
    delete ui;
}

void Tienda::mostrarPrecio(int indice)
{
    //Obtener el producto que se selecionado en el combo box
    float precioProducto = m_productos.at(indice)->precio();

    //Mostrar el precio en el widget correspondiente
    ui->outPrecio->setText("$ " + QString::number(precioProducto));
}

void Tienda::cargarDatos()
{
    //Vamos a crear y agregar porductos a la lista
    //m_productos.append(new Producto(1,"Pan", 0.15));
    //m_productos.append(new Producto(2,"Leche", 0.80));
    //m_productos.append(new Producto(3,"Huevos", 0.12));

    //CARGAR DATOS DEL ARCHIVO CSV
    QString pathActual = QDir::currentPath();
    qDebug() << pathActual;
    QFile archivo(pathActual + "/productos.csv");

    qDebug() << archivo.exists();

    bool primeraLinea = true;
    if(archivo.open(QFile::ReadOnly)){
        qDebug() << "Se abrio el archivo";
        QTextStream in(&archivo);
        while(!in.atEnd()){
            QString linea = in.readLine();

            if (primeraLinea){
                primeraLinea = false;
                continue;
            }
            //separar los datos con ';'
            QStringList datos = linea.split(";");
            int codigo = datos[0].toInt();
            float precio = datos[2].toFloat();
            //Creamos el producto
            m_productos.append(new Producto(codigo,datos[1],precio));
        }
    }else{
        QMessageBox msgBox;
        msgBox.setText("El archivo de productos no se puede abrir.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }
}

void Tienda::inicializarWidgets()
{
    /*Agrega los productos al combo box es decir en donde va a desplegar la
    lista de los productos*/
    for (int i=0; i < m_productos.length(); i++) {
        ui->inProducto->addItem(m_productos.at(i)->nombre());
    }

    //Colocar cabecera a la tabla
    QStringList cabecera = {"Cantidad", "Nombre", "Subtotal"};
    ui->outDetalle->setColumnCount(3);
    ui->outDetalle->setHorizontalHeaderLabels(cabecera);

    //Eventos
    connect(ui->inProducto, SIGNAL(currentIndexChanged(int)),
            this,SLOT(mostrarPrecio(int)));

    //Mostrar el precio del primer producto
    mostrarPrecio(0);
}

void Tienda::calcular(float subtotal)
{
    //Calcular valores
    m_subtotal += subtotal;
    float iva = m_subtotal * IVA;
    float total = m_subtotal + iva;
    //MOSTRAR VALORES EN EL UI
    ui->outSubtotal->setText(QString::number(m_subtotal));
    ui->outIVA->setText(QString::number(iva));
    ui->outTotal->setText(QString::number(total));

}

void Tienda::validarCedula()
{
    QString cedula = ui->inCedula->text();
    if(cedula.length() !=10){
        ui->inCedula->setStyleSheet("background-color: rgb(239, 41, 41);");
    }else{
        ui->inCedula->setStyleSheet("background-color: rgb(85, 255, 0);");
    }
}

void Tienda::validarNombre()
{
    QString nombre = ui->inNombre->text();
    if(nombre.length() ){
        ui->inCedula->setStyleSheet("background-color: rgb(239, 41, 41);");
    }else{
        ui->inCedula->setStyleSheet("background-color: rgb(85, 255, 0);");
    }
}

void Tienda::validarTelefono()
{
    QString telefono = ui->inTelefono->text();
    if(telefono.length() !=9 ){
        ui->inCedula->setStyleSheet("background-color: rgb(239, 41, 41);");
    }else{
        ui->inCedula->setStyleSheet("background-color: rgb(85, 255, 0);");
    }
}


void Tienda::on_cmdAgregar_clicked()
{
    //Obtener datos de la interfaz
    int indice = ui->inProducto->currentIndex();
    Producto *p = m_productos.at(indice);
    //Obtener la cantidad
    int cantidad = ui->inCantidad->value();

    //Validar: si la cantidad es 0; no hace nada
    if(cantidad == 0){
        return;
    }

    //Calcular el subtotal
    float subtotal = cantidad * p->precio();

    //Agregar a la tabla
    int posicion = ui->outDetalle->rowCount();
    ui->outDetalle->insertRow(posicion);
    ui->outDetalle->setItem(posicion,0,new QTableWidgetItem(QString::number(cantidad)));
    ui->outDetalle->setItem(posicion,1,new QTableWidgetItem(p->nombre()));
    ui->outDetalle->setItem(posicion,2,new QTableWidgetItem(QString::number(subtotal)));

    //Limpiar widgets
    ui->inCantidad->setValue(0);
    ui->inProducto->setFocus();

    //Calcular
    calcular(subtotal);

    //Condicion para no agregar el mismo producto
    if(m_productos == m_productos){
        //ui->outDetalle->setItem(posicion,0,new QTableWidgetItem(QString::number(cantidad + cantidad)));
        //ui->outDetalle.ed
    }
}

void Tienda::on_cmdFinalizar_clicked()
{
    QString CEDULA = ui->inCedula->text();
    QString NOMBRE = ui->inNombre->text();
    QString TOTAL = ui->outTotal->text();
    QString TELEFONO = ui->inTelefono->text();
    Acerca *dialogoAcerca = new Acerca();
    dialogoAcerca->setDato("\tTienda Doña Mechita"
                           "\n\nCliente: " + NOMBRE +
                           "\nNumero de Cedula: " + CEDULA +
                           "\nTelefono: " + TELEFONO +
                           "\nEl precio de su compra es: " + TOTAL);
    dialogoAcerca->actualizar();
    dialogoAcerca->show();
}

void Tienda::on_inCedula_editingFinished()
{
    validarCedula();
}
