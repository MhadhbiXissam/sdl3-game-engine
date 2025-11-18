

// -------------------------
// Node types
// -------------------------
typedef enum {
    NODE_TYPE_MODEL ; 
    NODE_TYPE_SCRIPT ; 
    NODE_TYPE_CAMERA ; 

} NodeType;

typedef struct {
    NodeType type ; 
    
} NodeArgInit ;  
typedef struct {
    NodeType type ; 
    
} NodeData ;

typedef struct {
    void* self ; 
    NodeType type ; 
    Node** childs ; 
    void* data ; 
    void (make_init*)(NodeArgInit*);
    void (make_free*)(void);
    
} Node ; 


typedef struct {
    void* game ; 
    Node** childs ; 
    int len ; 

} Scene ; 


typedef struct {
    NodeType typ ; 

}